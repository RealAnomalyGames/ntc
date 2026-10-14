#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRINTS 128
#define MAX_TEXT 1024
#define MAX_SOURCE_LINE 2048
#define MZ_HEADER_SIZE 32

typedef struct {
    char text[MAX_TEXT];
} PrintLine;

static PrintLine prints[MAX_PRINTS];
static unsigned print_count;
static unsigned exit_code;
static int saw_program;
static int saw_exit;
static int saw_end;

static void error(const char *message, unsigned line) {
    fprintf(stderr, "NTC error on line %u: %s\n", line, message);
}

static void put16(uint8_t *p, unsigned value) {
    p[0] = (uint8_t)(value & 0xFF);
    p[1] = (uint8_t)((value >> 8) & 0xFF);
}

static int is_name(const char *s) {
    if (!(*s) || !(isalpha((unsigned char)*s) || *s == '_'))
        return 0;

    for (++s; *s; ++s) {
        if (!(isalnum((unsigned char)*s) || *s == '_'))
            return 0;
    }
    return 1;
}

static char *trim(char *s) {
    while (isspace((unsigned char)*s))
        ++s;

    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1]))
        --end;
    *end = '\0';
    return s;
}

static int parse_source(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        perror(path);
        return 0;
    }

    char line[MAX_SOURCE_LINE];
    unsigned line_no = 0;

    while (fgets(line, sizeof line, f)) {
        ++line_no;
        char *s = trim(line);

        if (*s == '\0' || *s == ';')
            continue;

        if (saw_end) {
            error("Unexpected content after END.", line_no);
            fclose(f);
            return 0;
        }

        if (strncmp(s, "PROGRAM", 7) == 0 &&
            isspace((unsigned char)s[7])) {
            char *name = trim(s + 7);
            if (saw_program || !is_name(name)) {
                error("Expected one PROGRAM followed by a valid name.",
                      line_no);
                fclose(f);
                return 0;
            }
            saw_program = 1;
        } else if (strncmp(s, "PRINT", 5) == 0 &&
                   isspace((unsigned char)s[5])) {
            char *p = trim(s + 5);
            size_t len = strlen(p);

            if (!saw_program || saw_exit || *p != '"' ||
                len < 2 || p[len - 1] != '"') {
                error("Expected PRINT \"text\" before EXIT.", line_no);
                fclose(f);
                return 0;
            }

            if (print_count >= MAX_PRINTS ||
                len - 2 >= MAX_TEXT) {
                error("Too many PRINT statements or text is too long.",
                      line_no);
                fclose(f);
                return 0;
            }

            p[len - 1] = '\0';
            p++;

            if (strchr(p, '"') || strchr(p, '$')) {
                error("Text cannot contain quotes or the DOS '$' terminator.",
                      line_no);
                fclose(f);
                return 0;
            }

            strcpy(prints[print_count++].text, p);
        } else if (strncmp(s, "EXIT", 4) == 0 &&
                   isspace((unsigned char)s[4])) {
            char *p = trim(s + 4);
            char *endptr;
            long value;

            if (!saw_program || saw_exit || !*p) {
                error("EXIT must appear once after PROGRAM.", line_no);
                fclose(f);
                return 0;
            }

            value = strtol(p, &endptr, 10);
            if (*trim(endptr) != '\0' || value < 0 || value > 255) {
                error("EXIT requires a number from 0 to 255.", line_no);
                fclose(f);
                return 0;
            }

            exit_code = (unsigned)value;
            saw_exit = 1;
        } else if (strcmp(s, "END") == 0) {
            if (!saw_program || !saw_exit) {
                error("END requires a PROGRAM and EXIT.", line_no);
                fclose(f);
                return 0;
            }
            saw_end = 1;
        } else {
            error("Unknown instruction or invalid syntax.", line_no);
            fclose(f);
            return 0;
        }
    }

    if (ferror(f)) {
        perror("Reading source");
        fclose(f);
        return 0;
    }
    fclose(f);

    if (!saw_program || !saw_exit || !saw_end) {
        fprintf(stderr,
                "NTC error: source must contain PROGRAM, EXIT, and END.\n");
        return 0;
    }

    return 1;
}

static int compile_mz(const char *output_path) {
    /*
     * Each PRINT instruction:
     *   push cs       0E
     *   pop ds        1F
     *   mov dx, imm16 BA lo hi
     *   mov ah, 09    B4 09
     *   int 21h       CD 21
     *
     * DOS function 09h prints a '$'-terminated string.
     * EXIT uses DOS function 4Ch.
     */
    const unsigned print_instruction_size = 9;
    const unsigned exit_instruction_size = 5;

    unsigned code_size =
        print_count * print_instruction_size + exit_instruction_size;
    unsigned data_size = 0;

    for (unsigned i = 0; i < print_count; ++i)
        data_size += (unsigned)strlen(prints[i].text) + 3;

    unsigned image_size = code_size + data_size;
    unsigned file_size = MZ_HEADER_SIZE + image_size;

    if (image_size > 65535 || file_size > 65535) {
        fprintf(stderr, "NTC error: Build 001 output exceeds 16-bit limits.\n");
        return 0;
    }

    uint8_t *image = calloc(image_size ? image_size : 1, 1);
    if (!image) {
        perror("Memory allocation");
        return 0;
    }

    unsigned data_offset = code_size;
    unsigned pos = 0;

    for (unsigned i = 0; i < print_count; ++i) {
        unsigned message_offset = data_offset;

        image[pos++] = 0x0E;                    /* push cs */
        image[pos++] = 0x1F;                    /* pop ds */
        image[pos++] = 0xBA;                    /* mov dx, imm16 */
        put16(image + pos, message_offset);
        pos += 2;
        image[pos++] = 0xB4;                    /* mov ah, 09h */
        image[pos++] = 0x09;
        image[pos++] = 0xCD;                    /* int 21h */
        image[pos++] = 0x21;

        size_t len = strlen(prints[i].text);
        memcpy(image + data_offset, prints[i].text, len);
        data_offset += (unsigned)len;
        image[data_offset++] = '\r';
        image[data_offset++] = '\n';
        image[data_offset++] = '$';
    }

    image[pos++] = 0xB8;                        /* mov ax, 4Cxxh */
    put16(image + pos, 0x4C00 | exit_code);
    pos += 2;
    image[pos++] = 0xCD;                        /* int 21h */
    image[pos++] = 0x21;

    if (pos != code_size || data_offset != image_size) {
        fprintf(stderr, "NTC internal error: code size mismatch.\n");
        free(image);
        return 0;
    }

    uint8_t header[MZ_HEADER_SIZE] = {0};
    put16(header + 0, 0x5A4D);                 /* MZ signature */
    put16(header + 2, file_size % 512);         /* bytes in last page */
    put16(header + 4, (file_size + 511) / 512); /* file pages */
    put16(header + 6, 0);                       /* relocations */
    put16(header + 8, 2);                       /* header paragraphs */
    put16(header + 10, 0x1000);                 /* minimum extra memory */
    put16(header + 12, 0xFFFF);                /* maximum extra memory */
    put16(header + 14, 0);                      /* SS relative to image */
    put16(header + 16, 0xFFFE);                 /* SP */
    put16(header + 18, 0);                      /* checksum */
    put16(header + 20, 0);                      /* IP */
    put16(header + 22, 0);                      /* CS relative to image */
    put16(header + 24, 0x1C);                  /* relocation table offset */
    put16(header + 26, 0);                      /* overlay number */

    FILE *out = fopen(output_path, "wb");
    if (!out) {
        perror(output_path);
        free(image);
        return 0;
    }

    int ok = fwrite(header, 1, sizeof header, out) == sizeof header &&
             fwrite(image, 1, image_size, out) == image_size;

    if (fclose(out) != 0)
        ok = 0;

    free(image);

    if (!ok) {
        fprintf(stderr, "NTC error: Failed to write executable.\n");
        remove(output_path);
        return 0;
    }

    printf("NTC: DOS MZ executable created: %s\n", output_path);
    printf("NTC: %u PRINT instruction(s), %u bytes of code.\n",
           print_count, code_size);
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Neptune Code Compiler - Build 001\n");
        fprintf(stderr, "Usage: ntc <source.ntc> <output.exe>\n");
        return 1;
    }

    if (!parse_source(argv[1]))
        return 1;

    if (!compile_mz(argv[2]))
        return 1;

    return 0;
}
