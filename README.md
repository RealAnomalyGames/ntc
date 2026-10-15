<p align="center">
  <img src="https://github.com/RealAnomalyGames/ntc/blob/main/logo/nptlogo.png" alt="NTC Logo">
</p>

# Neptune Code (NTC)

**An assembly-inspired programming language by Neptune Corporation™**

Neptune Code (NTC) is a lightweight, instruction-based programming language designed to make low-level programming concepts accessible through a straightforward syntax.

NTC currently compiles source programs into **16-bit DOS MZ executables**, targeting compatible DOS environments. Its current features include numeric and text variables, arithmetic, keyboard input, text output, comparisons, conditional jumps, and loops built from labels and jumps.

---

## Features

- **Numeric variables** — unsigned 16-bit integer values from 0 to 65,535.
- **Text variables** — fixed-size text input and output.
- **Arithmetic** — addition, subtraction, multiplication, and integer division.
- **Keyboard input** — accept text or validated decimal numbers.
- **Text output** — display string literals and variable values.
- **Comparisons** — compare numeric operands.
- **Conditional branching** — jump based on comparison results.
- **Loops** — construct loops using labels and backward jumps.
- **DOS executable generation** — compile NTC source files into 16-bit DOS MZ executables.

## Quick Start

### Your first program

Create a file named `hello.ntc`:

```ntc
PROGRAM Hello

PRINT "Hello from Neptune Code!"
PRINT "My first NTC program."

EXIT 0
END
```

### Build the compiler

Requirements: GCC with C11 support.

From the repository root, run:

```bash
mkdir -p build
gcc -std=c11 -O2 -Wall -Wextra -Werror compiler/ntc.c -o build/ntc
```

### Compile your program

```bash
./build/ntc hello.ntc build/hello.exe
```

The resulting `build/hello.exe` is a **16-bit DOS executable**, not a native modern Windows application.

Transfer it into a compatible DOS environment, such as MS-DOS 6.22, and run:

```dos
HELLO
```

You can also compile the included examples using the NTC compiler.

## Example Programs

The `examples/` directory contains programs demonstrating the language's current features.

| Example | Description |
|---|---|
| `hello.ntc` | Basic text output |
| `variables.ntc` | Numeric variables and assignment |
| `arithmetic.ntc` | Arithmetic operations |
| `text_input.ntc` | Text keyboard input |
| `numeric_input.ntc` | Numeric keyboard input |
| `comparison.ntc` | Numeric comparisons |
| `branching.ntc` | Conditional branching |
| `countdown.ntc` | Repetition using labels and jumps |
| `skullgeon_demo.ntc` | An interactive dungeon demonstration |

Compile an example with:

```bash
./build/ntc examples/hello.ntc build/hello.exe
```

See [`docs/EXAMPLES.md`](docs/EXAMPLES.md) for the complete example catalog.

## Documentation

Start here to learn the language and understand its current limitations.

- [Getting Started](docs/GETTING_STARTED.md) — build the compiler and compile your first program.
- [Language Reference](docs/LANGUAGE_REFERENCE.md) — syntax, instructions, variables, and control flow.
- [Examples](docs/EXAMPLES.md) — descriptions and compilation instructions for included programs.
- [Current Limitations](docs/LIMITATIONS.md) — supported targets, data types, and restrictions.

## Build Validation

NTC uses GitHub Actions to build the compiler and validate example programs.

The build workflow can check that the compiler successfully generates DOS executables. Successful compilation alone does not guarantee that every program behaves correctly at runtime, so interactive examples should also be tested in a compatible DOS environment.

## Current Status

**Documentation release: Build 006 — Examples & Documentation**

Build 006 focuses on example programs and documentation. It does not introduce compiler changes beyond the existing Build 005 functionality.

The current compiler targets 16-bit DOS MZ executables. Other executable formats and additional language features are potential future development work, not current capabilities.

## Project Information

**Project:** Neptune Code (NTC)  
**Developer / Publisher:** Neptune Corporation™  
**Primary implementation language:** C  
**Compiler source:** [`compiler/ntc.c`](compiler/ntc.c)  
**Current executable target:** 16-bit DOS MZ

See the repository's license and distribution terms for the applicable permissions and restrictions.

---

*Neptune Code — straightforward programming, one instruction at a time.*
