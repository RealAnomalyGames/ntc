<p align="center">
  <img src="https://github.com/RealAnomalyGames/ntc/blob/main/logo/nptlogo.png" alt="NTC Logo">
</p>

# Neptune Code (NTC)

**NTC 0.1 — Official Release**

Neptune Code (NTC) is a lightweight, assembly-inspired programming language developed by Neptune Corporation™. Designed for simplicity and accessibility, NTC lets programmers create small programs that compile into executable applications for compatible 16-bit DOS environments.

NTC combines straightforward instructions, variables, arithmetic, user input, and conditional branching in a readable source-code format.

## Release Information

| Property | Details |
|---|---|
| Product | Neptune Code |
| Abbreviation | NTC |
| Version | 0.1 |
| Release status | Official release |
| Compiler implementation | C |
| Executable format | 16-bit DOS MZ EXE |
| Source extension | `.ntc` |
| Output extension | `.exe` |
| Web Compiler | Available through the NTC Web Compiler website |
| Developer | Neptune Corporation™ |

## Features

- **Simple syntax:** Write programs using readable, assembly-inspired instructions.
- **Variables:** Store numeric and text values.
- **Arithmetic:** Perform addition, subtraction, multiplication, and division.
- **User input:** Accept text and numeric input.
- **Output:** Display messages and variable values.
- **Labels and jumps:** Control program flow using labels and jump instructions.
- **Comparisons:** Compare numeric values and branch according to the result.
- **DOS executables:** Compile NTC source files into 16-bit DOS MZ executables.
- **Browser-based compilation:** Use the NTC Web Compiler without installing the native compiler locally.
- **Examples and documentation:** Learn the language through included sample programs and reference material.

## Getting Started

### Option 1: Use the NTC Web Compiler

Open the NTC Web Compiler website and select a `.ntc` source file from your computer.

1. Choose your NTC source file.
2. Click **Compile Program**.
3. Review the compiler output.
4. Download the generated `.exe` if compilation succeeds.
5. Run the executable in a compatible DOS environment.

Compilation takes place in your browser using the WebAssembly version of the NTC compiler.

### Option 2: Build the Native Compiler

You can build the compiler from source using a compatible C compiler.

For example, with GCC:

```bash
gcc -std=c11 -O2 -Wall -Wextra -Werror compiler/ntc.c -o build/ntc
```

Create the output directory first if it does not exist:

```bash
mkdir -p build
```

Compile an example program:

```bash
./build/ntc examples/hello.ntc build/hello.exe
```

Run the generated executable in a compatible DOS environment.

The commands above assume a Unix-like shell with GCC installed.

## Example Program

Save the following as `hello.ntc`:

```text
PROGRAM hello

PRINT "Hello from Neptune Code!"
PRINT "Welcome to NTC 0.1."

EXIT 0
END
```

Compile it using the NTC compiler. The resulting DOS executable displays the messages when run in a compatible environment.

## Language Reference

NTC 0.1 supports the following core instructions:

| Instruction | Purpose |
|---|---|
| `PROGRAM` | Declares the program |
| `LET` | Declares and initializes a variable |
| `MOV` | Assigns a numeric value |
| `ADD` | Adds a value |
| `SUB` | Subtracts a value |
| `MUL` | Multiplies a value |
| `DIV` | Divides a value |
| `PRINT` | Displays text or a variable |
| `INPUT` | Accepts user input |
| `LABEL` | Defines a jump target |
| `JMP` | Jumps to a label |
| `CMP` | Compares numeric values |
| `JE` | Jumps when values are equal |
| `JNE` | Jumps when values are not equal |
| `JA` | Jumps when the first value is above the second |
| `JB` | Jumps when the first value is below the second |
| `EXIT` | Sets the program's exit instruction and status |
| `END` | Marks the end of the program |

For full syntax, supported value types, restrictions, and examples, see the documentation in the `docs/` directory.

## Examples

The `examples/` directory contains sample programs demonstrating NTC's supported features, including:

- `hello.ntc` — basic text output
- `arithmetic.ntc` — arithmetic operations
- `variables.ntc` — variables and assignments
- `text_input.ntc` — text input
- `numeric_input.ntc` — numeric input
- `branching.ntc` — labels and jumps
- `comparison.ntc` — conditional comparisons
- `countdown.ntc` — a countdown program

Check the repository for the complete set of examples.

## Documentation

- [`GETTING_STARTED.md`](docs/GETTING_STARTED.md) — installation and first steps
- [`LANGUAGE_REFERENCE.md`](docs/LANGUAGE_REFERENCE.md) — language syntax and instructions
- [`EXAMPLES.md`](docs/EXAMPLES.md) — example program guide
- [`LIMITATIONS.md`](docs/LIMITATIONS.md) — known restrictions and compatibility information

## Compatibility and Limitations

NTC 0.1 targets **16-bit DOS MZ executables**. It is not a Windows PE compiler and does not directly produce native 32-bit or 64-bit Windows applications.

Programs must follow the language rules and limitations documented for this release. Numeric values use unsigned 16-bit arithmetic, and text input and supported string contents have restrictions.

Use a compatible DOS environment to run generated executables. Compatibility with other operating systems or DOS implementations should not be assumed unless tested.

## Project Status

**NTC 0.1 is officially released.**

The Build 007 release candidate has completed testing of the official `.ntc` examples, with each example successfully producing an executable.

Future versions may introduce additional instructions, compiler improvements, expanded platform support, and other features. Such additions are not part of NTC 0.1 unless explicitly documented.

## Credits

**Developed by Neptune Corporation™**

Neptune Code is part of the Neptune software ecosystem.

Copyright © Neptune Corporation™. All rights reserved.
