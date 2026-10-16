# NTC Current Limitations

This document describes the current capabilities and restrictions of Neptune Code (NTC).

**Executable format:** 16-bit DOS MZ

NTC is an evolving programming language. Features listed as planned or unsupported below should not be assumed to work.

## 1. Target Platform

The current compiler generates 16-bit DOS MZ executable files.

These programs are intended for compatible DOS environments, including the tested MS-DOS 6.22 environment.

The generated executables are not native 32-bit or 64-bit Windows PE applications. Modern 64-bit Windows does not natively execute 16-bit DOS applications.

## 2. Numeric Data

Numeric variables use unsigned 16-bit integer values.

- Minimum value: `0`
- Maximum value: `65535`
- Arithmetic uses integer operations.
- Results wrap modulo 65,536 where applicable.
- Division by zero is guarded against by the generated program.

There are no native floating-point numeric variables in the current language.

## 3. Text Data

Text variables use fixed-size buffers.

The current input buffer supports up to 127 characters. Text variables are supported by `INPUT` and `PRINT`, but general-purpose text manipulation is not available.

The current compiler does not support text-string comparisons using `CMP`.

The source parser also restricts certain characters inside string literals, including embedded double quotes and dollar signs.

## 4. Control Flow

The current compiler supports labels, unconditional jumps, and conditional jumps based on numeric comparisons.

Supported instructions include:

- `LABEL`
- `JMP`
- `CMP`
- `JE`
- `JNE`
- `JA`
- `JB`

Loops can be constructed using labels and backward jumps. There are no dedicated high-level `FOR` or `WHILE` statements.

Each program must contain exactly one final `EXIT` followed by `END`. Instructions and labels cannot be placed after `EXIT`.

## 5. Language Features Not Currently Provided

The current compiler does not provide native support for:

- User-defined functions or procedures
- Arrays or structures
- Floating-point variables
- General-purpose text comparisons
- A built-in file I/O API for NTC programs
- A built-in graphical user interface
- Native 32-bit or 64-bit PE executable output
- High-level loop statements such as `FOR` and `WHILE`

Do not assume these features are available unless a future release explicitly implements them.

## 6. Input Validation

Numeric input must be a valid decimal integer between `0` and `65535`. Invalid numeric input should be rejected by the generated program.

Text input is limited by the fixed-size buffer.

Interactive behavior should be tested in a compatible DOS environment before a program is distributed.

## 7. Compiler and Program Limits

The compiler implementation has internal limits on source lines, instructions, variables, labels, strings, and generated executable size. Very large programs may exceed these limits.

The compiler may reject unsupported syntax or invalid references during compilation.

## 8. Development Status

NTC is under active development. Build 006 focuses on examples and documentation and does not introduce compiler features.

Future targets such as Windows NE or 32-bit and 64-bit PE executables require additional compiler and runtime work. They are not part of the current Build 005 compiler baseline.

For the supported language syntax, see [LANGUAGE_REFERENCE.md](LANGUAGE_REFERENCE.md).
