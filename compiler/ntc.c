/*
 * NTC - Neptune Code
 * Build 005: Numeric Input
 *
 * Host compiler: C11
 * Output: 16-bit x86 DOS MZ executable
 *
 * Supported instructions:
 *   PROGRAM name
 *   LET name = value
 *   MOV destination, source
 *   ADD destination, operand
 *   SUB destination, operand
 *   MUL destination, operand
 *   DIV destination, operand
 *   PRINT "text"
 *   PRINT variable
 *   EXIT 0..255
 *   END
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARS       128
#define MAX_STRINGS    128
#define MAX_INSNS      512
#define MAX_NAME       64
#define MAX_TEXT       1024
#define NUMERIC_INPUT_STORAGE_SIZE TEXT_STORAGE_SIZE
#define MAX_LINE       2048
#define MAX_CODE       60000
#define MZ_HEADER_SIZE 32
#define MAX_LABELS     256
#define MAX_JUMPS      MAX_INSNS

typedef enum {
    VAR_NUMBER,
    VAR_TEXT
} VariableKind;

#define TEXT_BUFFER_SIZE 128
#define TEXT_STORAGE_SIZE (TEXT_BUFFER_SIZE + 2)

typedef struct {
    char name[MAX_NAME];
    VariableKind kind;
    unsigned offset;
} Variable;

typedef struct {
    char text[MAX_TEXT];
} StringData;

typedef enum {
    OP_LET,
    OP_MOV,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_PRINT_TEXT,
    OP_PRINT_VAR,
    OP_EXIT,
    OP_LABEL,
    OP_JMP,
    OP_CMP,
    OP_JE,
    OP_JNE,
    OP_JA,
    OP_JB,
    OP_INPUT,
    OP_PRINT_TEXT_VAR
} OpCode;

typedef enum {
    ARG_IMMEDIATE,
    ARG_VARIABLE
} ArgKind;

typedef struct {
    ArgKind kind;
    unsigned value;
} Argument;

typedef struct {
    OpCode op;
    unsigned destination;
    Argument argument;
    Argument left_argument;
    unsigned string_index;
    unsigned immediate;
    unsigned label_index;
} Instruction;

typedef enum {
    PATCH_VARIABLE,
    PATCH_STRING,
    PATCH_INPUT_BUFFER
} PatchKind;

typedef struct {
    unsigned position;
    PatchKind kind;
    unsigned index;
} AddressPatch;

typedef struct {
    char name[MAX_NAME];
    unsigned code_offset;
    int defined;
} Label;

typedef struct {
    unsigned position;
    unsigned label_index;
} JumpPatch;

static Variable variables[MAX_VARS];
static StringData strings[MAX_STRINGS];
static Instruction instructions[MAX_INSNS];
static AddressPatch patches[MAX_CODE / 2];

static Label labels[MAX_LABELS];
static JumpPatch jump_patches[MAX_JUMPS];

static unsigned label_count;
static unsigned jump_patch_count;

static unsigned variable_count;
static unsigned string_count;
static unsigned instruction_count;
static unsigned patch_count;
static unsigned exit_code;
static unsigned variable_bytes_used;

static uint8_t code[MAX_CODE];
static unsigned code_size;

static int saw_program;
static int saw_exit;
static int saw_end;

/* --------------------------------------------------------- */
/* General helpers                                           */
/* --------------------------------------------------------- */

static void error_at(unsigned line, const char *message)
{
    fprintf(stderr, "NTC error on line %u: %s\n", line, message);
}

static char *trim(char *s)
{
    while (isspace((unsigned char)*s))
        ++s;

    char *end = s + strlen(s);

    while (end > s && isspace((unsigned char)end[-1]))
        --end;

    *end = '\0';
    return s;
}

static int valid_name(const char *s)
{
    if (!*s ||
        !(isalpha((unsigned char)*s) || *s == '_'))
        return 0;

    for (++s; *s; ++s) {
        if (!(isalnum((unsigned char)*s) || *s == '_'))
            return 0;
    }

    return 1;
}

static int find_variable(const char *name)
{
    for (unsigned i = 0; i < variable_count; ++i) {
        if (strcmp(variables[i].name, name) == 0)
            return (int)i;
    }

    return -1;
}

static int get_or_create_variable(
    const char *name,
    VariableKind kind)
{
    int found = find_variable(name);

    if (found >= 0) {
        if (variables[found].kind != kind)
            return -1;

        return found;
    }

    if (variable_count >= MAX_VARS ||
        strlen(name) >= MAX_NAME)
        return -1;

    unsigned storage_size =
        kind == VAR_TEXT ? TEXT_STORAGE_SIZE : 2;

    if (variable_bytes_used + storage_size > 65535)
        return -1;

    unsigned index = variable_count++;

    strcpy(variables[index].name, name);
    variables[index].kind = kind;
    variables[index].offset = variable_bytes_used;

    variable_bytes_used += storage_size;

    return (int)index;
}

static int find_label(const char *name)
{
    for (unsigned i = 0; i < label_count; ++i) {
        if (strcmp(labels[i].name, name) == 0)
            return (int)i;
    }

    return -1;
}

static int get_or_create_label(const char *name)
{
    int found = find_label(name);

    if (found >= 0)
        return found;

    if (label_count >= MAX_LABELS ||
        strlen(name) >= MAX_NAME)
        return -1;

    unsigned index = label_count++;

    strcpy(labels[index].name, name);
    labels[index].code_offset = 0;
    labels[index].defined = 0;

    return (int)index;
}

static int parse_number(const char *s, unsigned *value)
{
    if (!*s)
        return 0;

    for (const char *p = s; *p; ++p) {
        if (!isdigit((unsigned char)*p))
            return 0;
    }

    char *end;
    unsigned long n = strtoul(s, &end, 10);

    if (*end || n > 65535)
        return 0;

    *value = (unsigned)n;
    return 1;
}

static int add_instruction(Instruction ins)
{
    if (instruction_count >= MAX_INSNS)
        return 0;

    instructions[instruction_count++] = ins;
    return 1;
}

static int add_string(const char *text)
{
    if (string_count >= MAX_STRINGS ||
        strlen(text) >= MAX_TEXT)
        return -1;

    unsigned index = string_count++;
    strcpy(strings[index].text, text);

    return (int)index;
}

/* Remove comments while respecting quoted strings. */
static void strip_comment(char *line)
{
    int quoted = 0;

    for (char *p = line; *p; ++p) {
        if (*p == '"')
            quoted = !quoted;
        else if (*p == ';' && !quoted) {
            *p = '\0';
            return;
        }
    }
}

/* Parse an operand as a previously declared variable or integer. */
static int parse_argument(char *text, Argument *arg)
{
    char *s = trim(text);

    if (valid_name(s)) {
        int index = find_variable(s);

        if (index < 0 ||
            variables[index].kind != VAR_NUMBER)
            return 0;

        arg->kind = ARG_VARIABLE;
        arg->value = (unsigned)index;
        return 1;
    }

    unsigned number;

    if (!parse_number(s, &number))
        return 0;

    arg->kind = ARG_IMMEDIATE;
    arg->value = number;
    return 1;
}

/* --------------------------------------------------------- */
/* Source parser                                             */
/* --------------------------------------------------------- */

static int parse_source(const char *path)
{
    FILE *file = fopen(path, "r");

    if (!file) {
        perror(path);
        return 0;
    }

    char line[MAX_LINE];
    unsigned line_number = 0;

    while (fgets(line, sizeof line, file)) {
        ++line_number;

        if (!strchr(line, '\n') && !feof(file)) {
            error_at(line_number, "Source line is too long.");
            fclose(file);
            return 0;
        }

        strip_comment(line);

        char *s = trim(line);

        if (!*s)
            continue;

        if (saw_end) {
            error_at(line_number, "Unexpected content after END.");
            fclose(file);
            return 0;
        }

        if (strncmp(s, "PROGRAM", 7) == 0 &&
            isspace((unsigned char)s[7])) {
            char *name = trim(s + 7);

            if (saw_program || !valid_name(name)) {
                error_at(line_number,
                         "Expected one PROGRAM followed by a valid name.");
                fclose(file);
                return 0;
            }

            saw_program = 1;
            continue;
        }

        if (!saw_program) {
            error_at(line_number, "PROGRAM must be the first instruction.");
            fclose(file);
            return 0;
        }

        if (saw_exit) {
            if (strcmp(s, "END") != 0) {
                error_at(line_number,
                         "EXIT must be the last instruction before END.");
                fclose(file);
                return 0;
            }

            saw_end = 1;
            continue;
        }

        if (strcmp(s, "END") == 0) {
            error_at(line_number, "END requires a preceding EXIT.");
            fclose(file);
            return 0;
        }

        Instruction ins = {0};

        /* LABEL name */
        if (strncmp(s, "LABEL", 5) == 0 &&
            isspace((unsigned char)s[5])) {
            char *name = trim(s + 5);

            if (!valid_name(name)) {
                error_at(line_number, "LABEL requires a valid name.");
                fclose(file);
                return 0;
            }

            int index = get_or_create_label(name);

            if (index < 0) {
                error_at(line_number, "Label table is full or name is too long.");
                fclose(file);
                return 0;
            }

            if (labels[index].defined) {
                error_at(line_number, "Duplicate label definition.");
                fclose(file);
                return 0;
            }

            labels[index].defined = 1;
            ins.op = OP_LABEL;
            ins.label_index = (unsigned)index;
        }

        /* JMP, JE, JNE, JA, JB label */
        else if ((strncmp(s, "JMP", 3) == 0 &&
                  isspace((unsigned char)s[3])) ||
                 (strncmp(s, "JNE", 3) == 0 &&
                  isspace((unsigned char)s[3])) ||
                 (strncmp(s, "JE", 2) == 0 &&
                  isspace((unsigned char)s[2])) ||
                 (strncmp(s, "JA", 2) == 0 &&
                  isspace((unsigned char)s[2])) ||
                 (strncmp(s, "JB", 2) == 0 &&
                  isspace((unsigned char)s[2]))) {
            char *target;
            char mnemonic[4] = {0};

            if (strncmp(s, "JMP", 3) == 0) {
                strcpy(mnemonic, "JMP");
                target = trim(s + 3);
                ins.op = OP_JMP;
            } else if (strncmp(s, "JNE", 3) == 0) {
                strcpy(mnemonic, "JNE");
                target = trim(s + 3);
                ins.op = OP_JNE;
            } else if (strncmp(s, "JE", 2) == 0) {
                strcpy(mnemonic, "JE");
                target = trim(s + 2);
                ins.op = OP_JE;
            } else if (strncmp(s, "JA", 2) == 0) {
                strcpy(mnemonic, "JA");
                target = trim(s + 2);
                ins.op = OP_JA;
            } else {
                strcpy(mnemonic, "JB");
                target = trim(s + 2);
                ins.op = OP_JB;
            }

            (void)mnemonic;

            if (!valid_name(target)) {
                error_at(line_number, "Jump requires a valid label name.");
                fclose(file);
                return 0;
            }

            int index = get_or_create_label(target);

            if (index < 0) {
                error_at(line_number, "Label table is full or name is too long.");
                fclose(file);
                return 0;
            }

            ins.label_index = (unsigned)index;
        }

        /* CMP left, right */
        else if (strncmp(s, "CMP", 3) == 0 &&
                 isspace((unsigned char)s[3])) {
            char *args = trim(s + 3);
            char *comma = strchr(args, ',');

            if (!comma) {
                error_at(line_number, "Expected CMP left, right.");
                fclose(file);
                return 0;
            }

            *comma = '\0';

            if (!parse_argument(trim(args), &ins.left_argument) ||
                !parse_argument(trim(comma + 1), &ins.argument)) {
                error_at(line_number,
                         "CMP requires declared variables or integers from 0 to 65535.");
                fclose(file);
                return 0;
            }

            ins.op = OP_CMP;
        }

        /* Existing LET, MOV, arithmetic, PRINT, and EXIT branches follow. */

        /* LET name = number OR LET name = "" */
        else if (strncmp(s, "LET", 3) == 0 &&
                 isspace((unsigned char)s[3])) {
            char *declaration = trim(s + 3);
            char *equals = strchr(declaration, '=');

            if (!equals) {
                error_at(line_number, "Expected LET name = value.");
                fclose(file);
                return 0;
            }

            *equals = '\0';

            char *name = trim(declaration);
            char *value_text = trim(equals + 1);

            if (!valid_name(name)) {
                error_at(line_number, "LET requires a valid variable name.");
                fclose(file);
                return 0;
            }

            /* Text variables are currently declared with an empty string. */
            if (strcmp(value_text, "\"\"") == 0) {
                int index = get_or_create_variable(name, VAR_TEXT);

                if (index < 0) {
                    error_at(line_number,
                             "Cannot create text variable: table full, "
                             "name too long, or variable type conflict.");
                    fclose(file);
                    return 0;
                }

                ins.op = OP_LET;
                ins.destination = (unsigned)index;
                ins.immediate = 0;
            } else {
                unsigned value;

                if (!parse_number(value_text, &value)) {
                    error_at(line_number,
                             "LET requires an integer from 0 to 65535 "
                             "or an empty text value \"\".");
                    fclose(file);
                    return 0;
                }

                int index = get_or_create_variable(name, VAR_NUMBER);

                if (index < 0) {
                    error_at(line_number,
                             "Cannot create numeric variable: table full, "
                             "name too long, or variable type conflict.");
                    fclose(file);
                    return 0;
                }

                ins.op = OP_LET;
                ins.destination = (unsigned)index;
                ins.immediate = value;
            }
        }

        /* MOV destination, source */
        else if (strncmp(s, "MOV", 3) == 0 &&
                 isspace((unsigned char)s[3])) {
            char *args = trim(s + 3);
            char *comma = strchr(args, ',');

            if (!comma) {
                error_at(line_number, "Expected MOV destination, source.");
                fclose(file);
                return 0;
            }

            *comma = '\0';

            char *destination = trim(args);
            char *source = trim(comma + 1);

            int index = find_variable(destination);

            if (index < 0 ||
                variables[index].kind != VAR_NUMBER ||
                !parse_argument(source, &ins.argument)) {
                error_at(line_number,
                         "MOV requires a declared destination and valid source.");
                fclose(file);
                return 0;
            }

            ins.op = OP_MOV;
            ins.destination = (unsigned)index;
        }

        /* ADD, SUB, MUL, DIV destination, operand */
        else if ((strncmp(s, "ADD", 3) == 0 &&
                  isspace((unsigned char)s[3])) ||
                 (strncmp(s, "SUB", 3) == 0 &&
                  isspace((unsigned char)s[3])) ||
                 (strncmp(s, "MUL", 3) == 0 &&
                  isspace((unsigned char)s[3])) ||
                 (strncmp(s, "DIV", 3) == 0 &&
                  isspace((unsigned char)s[3]))) {
            char *args = trim(s + 3);
            char *comma = strchr(args, ',');

            if (!comma) {
                error_at(line_number,
                         "Expected arithmetic instruction destination, operand.");
                fclose(file);
                return 0;
            }

            *comma = '\0';

            char *destination = trim(args);
            char *operand = trim(comma + 1);

            int index = find_variable(destination);

            if (index < 0 ||
                variables[index].kind != VAR_NUMBER ||
                !parse_argument(operand, &ins.argument)) {
                error_at(line_number,
                         "Arithmetic requires a declared destination and valid operand.");
                fclose(file);
                return 0;
            }

            if (s[0] == 'A')
                ins.op = OP_ADD;
            else if (s[0] == 'S')
                ins.op = OP_SUB;
            else if (s[0] == 'M')
                ins.op = OP_MUL;
            else
                ins.op = OP_DIV;

            if (ins.op == OP_DIV &&
                ins.argument.kind == ARG_IMMEDIATE &&
                ins.argument.value == 0) {
                error_at(line_number, "Division by zero is not allowed.");
                fclose(file);
                return 0;
            }

            ins.destination = (unsigned)index;
        }

        /* PRINT "text" or PRINT variable */
        else if (strncmp(s, "PRINT", 5) == 0 &&
                 isspace((unsigned char)s[5])) {
            char *value = trim(s + 5);
            size_t len = strlen(value);

            if (len >= 2 && value[0] == '"' &&
                value[len - 1] == '"') {
                value[len - 1] = '\0';

                if (strchr(value + 1, '"') ||
                    strchr(value + 1, '$')) {
                    error_at(line_number,
                             "PRINT text cannot contain quotes or '$'.");
                    fclose(file);
                    return 0;
                }

                int index = add_string(value + 1);

                if (index < 0) {
                    error_at(line_number, "String table is full or text is too long.");
                    fclose(file);
                    return 0;
                }

                ins.op = OP_PRINT_TEXT;
                ins.string_index = (unsigned)index;
            } else {
                int index = find_variable(value);

                if (index < 0) {
                    error_at(line_number,
                             "PRINT requires quoted text or a declared variable.");
                    fclose(file);
                    return 0;
                }

                ins.destination = (unsigned)index;

                if (variables[index].kind == VAR_TEXT)
                    ins.op = OP_PRINT_TEXT_VAR;
                else
                    ins.op = OP_PRINT_VAR;
            }
        }

        /* INPUT text_variable OR INPUT numeric_variable */
        else if (strncmp(s, "INPUT", 5) == 0 &&
                 isspace((unsigned char)s[5])) {
            char *name = trim(s + 5);
            int index = find_variable(name);

            if (!valid_name(name) || index < 0) {
                error_at(line_number,
                         "INPUT requires a previously declared variable.");
                fclose(file);
                return 0;
            }

            ins.op = OP_INPUT;
            ins.destination = (unsigned)index;

            if (variables[index].kind == VAR_NUMBER) {
                int message_index = add_string(
                    "Invalid input. Enter a number from 0 to 65535:");

                if (message_index < 0) {
                    error_at(line_number,
                             "Could not add numeric input error message.");
                    fclose(file);
                    return 0;
                }

                ins.string_index = (unsigned)message_index;
            }
        }

        /* EXIT code */
        else if (strncmp(s, "EXIT", 4) == 0 &&
                 isspace((unsigned char)s[4])) {
            unsigned value;

            if (!parse_number(trim(s + 4), &value) || value > 255) {
                error_at(line_number, "EXIT requires an integer from 0 to 255.");
                fclose(file);
                return 0;
            }

            ins.op = OP_EXIT;
            ins.immediate = value;
            exit_code = value;
            saw_exit = 1;
        } else {
            error_at(line_number, "Unknown instruction or invalid syntax.");
            fclose(file);
            return 0;
        }

        if (!add_instruction(ins)) {
            error_at(line_number, "Instruction limit exceeded.");
            fclose(file);
            return 0;
        }
    }

    int read_error = ferror(file);
    fclose(file);

    if (read_error) {
        fprintf(stderr, "NTC error: Could not read source file.\n");
        return 0;
    }

    if (!saw_program || !saw_exit || !saw_end) {
        fprintf(stderr,
                "NTC error: Source must contain PROGRAM, EXIT, and END.\n");
        return 0;
    }

    for (unsigned i = 0; i < label_count; ++i) {
        if (!labels[i].defined) {
            fprintf(stderr,
                    "NTC error: Undefined label '%s'.\n",
                    labels[i].name);
            return 0;
        }
    }

    return 1;
}

/* --------------------------------------------------------- */
/* 8086 code emitter                                         */
/* --------------------------------------------------------- */

static int emit8(unsigned value)
{
    if (code_size >= MAX_CODE)
        return 0;

    code[code_size++] = (uint8_t)value;
    return 1;
}

static int emit16(unsigned value)
{
    return emit8(value) && emit8(value >> 8);
}

static int emit_bytes(const uint8_t *bytes, unsigned count)
{
    for (unsigned i = 0; i < count; ++i) {
        if (!emit8(bytes[i]))
            return 0;
    }

    return 1;
}

static int emit_address(PatchKind kind, unsigned index)
{
    if (patch_count >= MAX_CODE / 2)
        return 0;

    patches[patch_count].position = code_size;
    patches[patch_count].kind = kind;
    patches[patch_count].index = index;
    ++patch_count;

    return emit16(0);
}

static int emit_variable_load(unsigned index)
{
    return emit8(0xA1) &&
           emit_address(PATCH_VARIABLE, index);
}

static int emit_variable_store(unsigned index)
{
    return emit8(0xA3) &&
           emit_address(PATCH_VARIABLE, index);
}

static int emit_input(unsigned index)
{
    /*
     * DOS INT 21h, AH=0Ah:
     * DS:DX points to the buffered keyboard input structure.
     *
     * Buffer layout:
     *   +0: maximum buffer capacity
     *   +1: number of characters entered
     *   +2: input characters, followed by carriage return
     */
    return emit8(0xBA) &&                  /* mov dx, imm16 */
           emit_address(PATCH_VARIABLE, index) &&
           emit8(0xB4) &&                  /* mov ah, 0Ah */
           emit8(0x0A) &&
           emit8(0xCD) &&                  /* int 21h */
           emit8(0x21);
}

static int patch_short_jump(unsigned displacement_position,
                            unsigned target)
{
    int displacement =
        (int)target - (int)(displacement_position + 1);

    if (displacement < -128 || displacement > 127)
        return 0;

    code[displacement_position] = (uint8_t)(int8_t)displacement;
    return 1;
}

static int emit_numeric_input(unsigned index, unsigned message_index)
{
    unsigned retry = code_size;

    /* DOS buffered keyboard input: DS:DX -> numeric buffer. */
    if (!emit8(0xBA) ||
        !emit_address(PATCH_INPUT_BUFFER, 0) ||
        !emit8(0xB4) || !emit8(0x0A) ||
        !emit8(0xCD) || !emit8(0x21))
        return 0;

    /* Read the number of characters entered. */
    if (!emit8(0x8A) || !emit8(0x0E) ||
        !emit_address(PATCH_INPUT_BUFFER, 1))
        return 0;

    /* Empty input: retry. */
    if (!emit8(0xE3))
        return 0;

    unsigned empty_retry = code_size;
    if (!emit8(0))
        return 0;

    /* SI points to the first character; DI counts remaining digits. */
    if (!emit8(0xBE) ||
        !emit_address(PATCH_INPUT_BUFFER, 2) ||
        !emit8(0x31) || !emit8(0xDB) ||
        !emit8(0x89) || !emit8(0xCF))
        return 0;

    unsigned parse_loop = code_size;

    /* AL = current character minus '0'; reject anything above 9. */
    if (!emit8(0x8A) || !emit8(0x04) ||
        !emit8(0x2C) || !emit8(0x30) ||
        !emit8(0x3C) || !emit8(0x09) ||
        !emit8(0x77))
        return 0;

    unsigned bad_digit = code_size;
    if (!emit8(0))
        return 0;

    /* Save digit, multiply accumulated value by ten. */
    if (!emit8(0x30) || !emit8(0xE4) ||
        !emit8(0x50) ||
        !emit8(0x89) || !emit8(0xD8) ||
        !emit8(0xB9) || !emit16(10) ||
        !emit8(0xF7) || !emit8(0xE1) ||
        !emit8(0x59))
        return 0;

    /* A nonzero high word means multiplication overflowed. */
    if (!emit8(0x09) || !emit8(0xD2) ||
        !emit8(0x75))
        return 0;

    unsigned multiply_overflow = code_size;
    if (!emit8(0))
        return 0;

    /* Add digit; carry means the result exceeds 65535. */
    if (!emit8(0x01) || !emit8(0xC8) ||
        !emit8(0x72))
        return 0;

    unsigned addition_overflow = code_size;
    if (!emit8(0))
        return 0;

    if (!emit8(0x89) || !emit8(0xC3) ||
        !emit8(0x46) ||
        !emit8(0x4F) ||
        !emit8(0x75))
        return 0;

    unsigned continue_loop = code_size;
    if (!emit8(0))
        return 0;

    /* Success: store the parsed number. */
    if (!emit8(0x89) || !emit8(0xD8) ||
        !emit_variable_store(index) ||
        !emit8(0xEB))
        return 0;

    unsigned success_jump = code_size;
    if (!emit8(0))
        return 0;

    /* Invalid input: display the message and read again. */
    unsigned invalid = code_size;

    if (!emit8(0xBA) ||
        !emit_address(PATCH_STRING, message_index) ||
        !emit8(0xB4) || !emit8(0x09) ||
        !emit8(0xCD) || !emit8(0x21) ||
        !emit8(0xE9))
        return 0;

    /* Backward near jump to retry. */
    int retry_displacement =
        (int)retry - (int)(code_size + 2);

    if (retry_displacement < -32768 ||
        retry_displacement > 32767 ||
        !emit16((unsigned)retry_displacement))
        return 0;

    unsigned done = code_size;

    return patch_short_jump(empty_retry, retry) &&
           patch_short_jump(bad_digit, invalid) &&
           patch_short_jump(multiply_overflow, invalid) &&
           patch_short_jump(addition_overflow, invalid) &&
           patch_short_jump(continue_loop, parse_loop) &&
           patch_short_jump(success_jump, done);
}

static int emit_print_text_variable(unsigned index)
{
    /*
     * SI = address of the DOS input buffer.
     * CX = character count stored at [SI+1].
     * DX = address of text stored at [SI+2].
     *
     * DOS INT 21h, AH=40h writes exactly CX characters
     * to standard output, so '$' characters are allowed.
     */

    if (!emit8(0xBE) ||                    /* mov si, imm16 */
        !emit_address(PATCH_VARIABLE, index))
        return 0;

    static const uint8_t setup[] = {
        0x31, 0xC9,             /* xor cx, cx */
        0x8A, 0x4C, 0x01,       /* mov cl, [si+1] */
        0x8D, 0x54, 0x02,       /* lea dx, [si+2] */
        0xBB, 0x01, 0x00,       /* mov bx, 1 (stdout) */
        0xB4, 0x40,             /* mov ah, 40h */
        0xCD, 0x21              /* int 21h */
    };

    if (!emit_bytes(setup, sizeof setup))
        return 0;

    /* Print CR/LF after the text, matching PRINT's other forms. */
    static const uint8_t newline[] = {
        0xB2, 0x0D,             /* mov dl, 13 */
        0xB4, 0x02,             /* mov ah, 02h */
        0xCD, 0x21,
        0xB2, 0x0A,             /* mov dl, 10 */
        0xB4, 0x02,
        0xCD, 0x21
    };

    return emit_bytes(newline, sizeof newline);
}

static int emit_near_jump(unsigned label_index)
{
    if (jump_patch_count >= MAX_JUMPS)
        return 0;

    if (!emit8(0xE9))
        return 0; /* JMP near, relative 16-bit displacement */

    jump_patches[jump_patch_count].position = code_size;
    jump_patches[jump_patch_count].label_index = label_index;
    ++jump_patch_count;

    return emit16(0);
}

static int emit_conditional_jump(
    uint8_t inverse_condition,
    unsigned label_index)
{
    /*
     * Invert the short condition to skip the three-byte
     * near JMP when the original condition is false.
     */
    return emit8(inverse_condition) &&
           emit8(0x03) &&
           emit_near_jump(label_index);
}

static int emit_comparison(const Instruction *ins)
{
    /* Load the left operand into AX. */
    if (ins->left_argument.kind == ARG_VARIABLE) {
        if (!emit_variable_load(ins->left_argument.value))
            return 0;
    } else {
        if (!emit8(0xB8) ||
            !emit16(ins->left_argument.value))
            return 0;
    }

    /* Compare AX with the right operand. */
    if (ins->argument.kind == ARG_IMMEDIATE) {
        /* cmp ax, immediate16 */
        return emit8(0x3D) &&
               emit16(ins->argument.value);
    }

    /* cmp ax, word ptr [variable] */
    return emit8(0x3B) &&
           emit8(0x06) &&
           emit_address(PATCH_VARIABLE, ins->argument.value);
}

/* --------------------------------------------------------- */
/* Decimal number output                                     */
/* --------------------------------------------------------- */

/*
 * AX contains the unsigned number.
 *
 * Divide repeatedly by ten, pushing remainders.
 * Pop and print digits in reverse order.
 *
 * Uses DOS INT 21h, AH=02h to output each character.
 */
static int emit_print_number(void)
{
    static const uint8_t prefix[] = {
        0xBB, 0x0A, 0x00,       /* mov bx, 10 */
        0x31, 0xC9              /* xor cx, cx */
    };

    if (!emit_bytes(prefix, sizeof prefix))
        return 0;

    unsigned divide_loop = code_size;

    static const uint8_t divide_body[] = {
        0x31, 0xD2,             /* xor dx, dx */
        0xF7, 0xF3,             /* div bx */
        0x52,                   /* push dx */
        0x41,                   /* inc cx */
        0x85, 0xC0              /* test ax, ax */
    };

    if (!emit_bytes(divide_body, sizeof divide_body))
        return 0;

    if (!emit8(0x75))
        return 0;

    unsigned jne_position = code_size;

    if (!emit8(0))
        return 0;

    int back = (int)divide_loop - (int)code_size;

    if (back < -128 || back > 127)
        return 0;

    code[jne_position] = (uint8_t)(int8_t)back;

    unsigned print_loop = code_size;

    static const uint8_t print_body[] = {
        0x5A,                   /* pop dx */
        0x80, 0xC2, 0x30,       /* add dl, '0' */
        0xB4, 0x02,             /* mov ah, 02h */
        0xCD, 0x21              /* int 21h */
    };

    if (!emit_bytes(print_body, sizeof print_body))
        return 0;

    if (!emit8(0xE2))
        return 0;

    unsigned loop_position = code_size;

    if (!emit8(0))
        return 0;

    unsigned distance = code_size - print_loop;

    if (distance > 128)
        return 0;

    code[loop_position] = (uint8_t)(int8_t)-(int)distance;

    /* Print CR/LF after the decimal number. */
    static const uint8_t newline[] = {
        0xB2, 0x0D,             /* mov dl, 13 */
        0xB4, 0x02,
        0xCD, 0x21,
        0xB2, 0x0A,             /* mov dl, 10 */
        0xB4, 0x02,
        0xCD, 0x21
    };

    return emit_bytes(newline, sizeof newline);
}

/* --------------------------------------------------------- */
/* Instruction code generation                               */
/* --------------------------------------------------------- */

static int emit_instruction(const Instruction *ins)
{
    switch (ins->op) {
    case OP_LET:
        if (variables[ins->destination].kind == VAR_TEXT) {
            /* Text buffer initialization is performed in compile_mz(). */
            return 1;
        }

        /* mov ax, immediate; mov [variable], ax */
        return emit8(0xB8) &&
               emit16(ins->immediate) &&
               emit_variable_store(ins->destination);

    case OP_MOV:
        if (ins->argument.kind == ARG_VARIABLE) {
            return emit_variable_load(ins->argument.value) &&
                   emit_variable_store(ins->destination);
        }

        return emit8(0xB8) &&
               emit16(ins->argument.value) &&
               emit_variable_store(ins->destination);

    case OP_ADD:
    case OP_SUB:
        if (!emit_variable_load(ins->destination))
            return 0;

        if (ins->argument.kind == ARG_IMMEDIATE) {
            if (!emit8(ins->op == OP_ADD ? 0x05 : 0x2D) ||
                !emit16(ins->argument.value))
                return 0;
        } else {
            if (!emit8(ins->op == OP_ADD ? 0x03 : 0x2B) ||
                !emit8(0x06) ||
                !emit_address(PATCH_VARIABLE, ins->argument.value))
                return 0;
        }

        return emit_variable_store(ins->destination);

    case OP_MUL:
        if (!emit_variable_load(ins->destination))
            return 0;

        if (ins->argument.kind == ARG_IMMEDIATE) {
            if (!emit8(0xBB) ||
                !emit16(ins->argument.value) ||
                !emit8(0xF7) ||
                !emit8(0xE3))
                return 0;
        } else {
            if (!emit8(0xF7) ||
                !emit8(0x26) ||
                !emit_address(PATCH_VARIABLE, ins->argument.value))
                return 0;
        }

        /* Store the low 16 bits: arithmetic wraps at 65536. */
        return emit_variable_store(ins->destination);

    case OP_DIV:
        if (!emit_variable_load(ins->destination))
            return 0;

        if (ins->argument.kind == ARG_IMMEDIATE) {
            if (!emit8(0xBB) ||
                !emit16(ins->argument.value) ||
                !emit8(0x31) ||
                !emit8(0xD2) ||
                !emit8(0xF7) ||
                !emit8(0xF3))
                return 0;
        } else {
            /*
             * cmp word [operand], 0
             * jne +5
             * mov ax, 4C01h
             * int 21h
             */
            if (!emit8(0x83) ||
                !emit8(0x3E) ||
                !emit_address(PATCH_VARIABLE, ins->argument.value) ||
                !emit8(0x00) ||
                !emit8(0x75) ||
                !emit8(0x05) ||
                !emit8(0xB8) ||
                !emit16(0x4C01) ||
                !emit8(0xCD) ||
                !emit8(0x21))
                return 0;

            if (!emit8(0x31) ||
                !emit8(0xD2) ||
                !emit8(0xF7) ||
                !emit8(0x36) ||
                !emit_address(PATCH_VARIABLE, ins->argument.value))
                return 0;
        }

        return emit_variable_store(ins->destination);

    case OP_PRINT_TEXT:
        /* mov dx, string address; mov ah, 09h; int 21h */
        if (!emit8(0xBA))
            return 0;
        if (!emit_address(PATCH_STRING, ins->string_index))
            return 0;
        if (!emit8(0xB4))
            return 0;
        if (!emit8(0x09))
            return 0;
        if (!emit8(0xCD))
            return 0;
        if (!emit8(0x21))
            return 0;
        return 1;

    case OP_PRINT_VAR:
        return emit_variable_load(ins->destination) &&
               emit_print_number();

    case OP_INPUT:
        if (variables[ins->destination].kind == VAR_TEXT)
            return emit_input(ins->destination);

        return emit_numeric_input(
            ins->destination,
            ins->string_index);

    case OP_PRINT_TEXT_VAR:
        return emit_print_text_variable(ins->destination);

    case OP_EXIT:
        /* mov ax, 4Cxxh; int 21h */
        return emit8(0xB8) &&
               emit16(0x4C00 | ins->immediate) &&
               emit8(0xCD) &&
               emit8(0x21);

    case OP_LABEL:
        labels[ins->label_index].code_offset = code_size;
        return 1;

    case OP_JMP:
        return emit_near_jump(ins->label_index);

    case OP_CMP:
        return emit_comparison(ins);

    case OP_JE:
        /* If not equal, skip the JMP. */
        return emit_conditional_jump(0x75, ins->label_index);

    case OP_JNE:
        /* If equal, skip the JMP. */
        return emit_conditional_jump(0x74, ins->label_index);

    case OP_JA:
        /* If below or equal, skip the JMP. */
        return emit_conditional_jump(0x76, ins->label_index);

    case OP_JB:
        /* If above or equal, skip the JMP. */
        return emit_conditional_jump(0x73, ins->label_index);
    }

    return 0;
}

/* --------------------------------------------------------- */
/* DOS MZ executable generation                              */
/* --------------------------------------------------------- */

static void put16(uint8_t *p, unsigned value)
{
    p[0] = (uint8_t)(value & 0xFF);
    p[1] = (uint8_t)((value >> 8) & 0xFF);
}

static int compile_mz(const char *output_path)
{
    /* Initialize DS to the program's code segment. */
    if (!emit8(0x0E) || !emit8(0x1F)) {
        fprintf(stderr, "NTC error: Code buffer exhausted.\n");
        return 0;
    }

    for (unsigned i = 0; i < instruction_count; ++i) {
        if (!emit_instruction(&instructions[i])) {
            fprintf(stderr,
                    "NTC error: Could not generate instruction %u "
                    "(opcode %d, code size %u, patches %u).\n",
                    i + 1,
                    (int)instructions[i].op,
                    code_size,
                    patch_count);
            return 0;
        }
    }

    /* Resolve all near jumps after every label has a code offset. */
    for (unsigned i = 0; i < jump_patch_count; ++i) {
        JumpPatch *patch = &jump_patches[i];

        if (patch->label_index >= label_count ||
            !labels[patch->label_index].defined) {
            fprintf(stderr, "NTC internal error: Invalid jump target.\n");
            return 0;
        }

        int displacement =
            (int)labels[patch->label_index].code_offset -
            (int)(patch->position + 2);

        if (displacement < -32768 || displacement > 32767) {
            fprintf(stderr,
                    "NTC error: Jump target '%s' is out of range.\n",
                    labels[patch->label_index].name);
            return 0;
        }

        put16(code + patch->position, (unsigned)displacement);
    }

    unsigned variable_bytes =
        variable_bytes_used + NUMERIC_INPUT_STORAGE_SIZE;
    unsigned string_bytes = 0;

    for (unsigned i = 0; i < string_count; ++i)
        string_bytes += (unsigned)strlen(strings[i].text) + 3;

    unsigned image_size = code_size + variable_bytes + string_bytes;
    unsigned file_size = MZ_HEADER_SIZE + image_size;

    /*
     * The initial implementation keeps the entire load image
     * within one 16-bit segment.
     */
    if (image_size > 65535 || file_size > 65535) {
        fprintf(stderr, "NTC error: Executable exceeds Build 004 size limits.\n");
        return 0;
    }

    /* Resolve code references to variables and strings. */
    for (unsigned i = 0; i < patch_count; ++i) {
        AddressPatch *patch = &patches[i];
        unsigned address;

        if (patch->kind == PATCH_VARIABLE) {
            if (patch->index >= variable_count) {
                fprintf(stderr,
                        "NTC internal error: Invalid variable reference.\n");
                return 0;
            }

            address = code_size + variables[patch->index].offset;
        } else if (patch->kind == PATCH_INPUT_BUFFER) {
            if (patch->index >= NUMERIC_INPUT_STORAGE_SIZE) {
                fprintf(stderr,
                        "NTC internal error: Invalid input buffer reference.\n");
                return 0;
            }

            address = code_size + variable_bytes_used + patch->index;
        } else {
            if (patch->index >= string_count) {
                fprintf(stderr,
                        "NTC internal error: Invalid string reference.\n");
                return 0;
            }

            address = code_size + variable_bytes;

            for (unsigned j = 0; j < patch->index; ++j)
                address += (unsigned)strlen(strings[j].text) + 3;
        }

        if (address > 65535 || patch->position + 1 >= code_size) {
            fprintf(stderr, "NTC internal error: Address out of range.\n");
            return 0;
        }

        put16(code + patch->position, address);
    }

    uint8_t *image = calloc(image_size ? image_size : 1, 1);

    if (!image) {
        perror("Memory allocation");
        return 0;
    }

    memcpy(image, code, code_size);

    /* Initialize the maximum input length for each text variable. */
    for (unsigned i = 0; i < variable_count; ++i) {
        if (variables[i].kind == VAR_TEXT) {
            unsigned buffer_offset = code_size + variables[i].offset;

            image[buffer_offset] = TEXT_BUFFER_SIZE;
            image[buffer_offset + 1] = 0;
        }
    }

    unsigned numeric_buffer_offset =
        code_size + variable_bytes_used;

    image[numeric_buffer_offset] = TEXT_BUFFER_SIZE;
    image[numeric_buffer_offset + 1] = 0;

    unsigned offset = code_size + variable_bytes;

    for (unsigned i = 0; i < string_count; ++i) {
        size_t len = strlen(strings[i].text);

        memcpy(image + offset, strings[i].text, len);
        offset += (unsigned)len;

        image[offset++] = '\r';
        image[offset++] = '\n';
        image[offset++] = '$';
    }

    if (offset != image_size) {
        fprintf(stderr, "NTC internal error: Image size mismatch.\n");
        free(image);
        return 0;
    }

    uint8_t header[MZ_HEADER_SIZE] = {0};

    put16(header + 0, 0x5A4D);                  /* MZ */
    put16(header + 2, file_size % 512);
    put16(header + 4, (file_size + 511) / 512);
    put16(header + 6, 0);                       /* Relocations */
    put16(header + 8, 2);                       /* Header paragraphs */
    put16(header + 10, 0x1000);                 /* Minimum extra memory */
    put16(header + 12, 0xFFFF);                 /* Maximum extra memory */
    put16(header + 14, 0);                       /* SS */
    put16(header + 16, 0xFFFE);                 /* SP */
    put16(header + 18, 0);                       /* Checksum */
    put16(header + 20, 0);                       /* IP */
    put16(header + 22, 0);                       /* CS */
    put16(header + 24, 0x1C);                   /* Relocation table */
    put16(header + 26, 0);                       /* Overlay */

    FILE *out = fopen(output_path, "wb");

    if (!out) {
        perror(output_path);
        free(image);
        return 0;
    }

    int ok =
        fwrite(header, 1, sizeof header, out) == sizeof header &&
        fwrite(image, 1, image_size, out) == image_size;

    if (fclose(out) != 0)
        ok = 0;

    free(image);

    if (!ok) {
        fprintf(stderr, "NTC error: Failed to write executable.\n");
        remove(output_path);
        return 0;
    }

    printf("NTC Build 004: Created %s\n", output_path);
    printf("Variables: %u\n", variable_count);
    printf("Instructions: %u\n", instruction_count);
    printf("Code: %u bytes; data: %u bytes\n",
           code_size, variable_bytes + string_bytes);

    return 1;
}

/* --------------------------------------------------------- */
/* Main                                                      */
/* --------------------------------------------------------- */

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "NTC - Neptune Code Compiler, Build 004\n");
        fprintf(stderr, "Usage: ntc <source.ntc> <output.exe>\n");
        return 1;
    }

    if (!parse_source(argv[1]))
        return 1;

    if (!compile_mz(argv[2]))
        return 1;

    return 0;
}
