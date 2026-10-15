# Getting Started with Neptune Code (NTC)

Welcome to **Neptune Code (NTC)**, a simple, assembly-inspired programming language developed by Neptune Corporation™. NTC is designed to make low-level programming concepts accessible through a straightforward instruction-based syntax.

NTC currently compiles programs into **16-bit DOS MZ executable files**, intended for compatible DOS environments.

## Requirements

To build the NTC compiler from source, you need:

- GCC or another compatible C compiler capable of compiling the compiler source.
- A C11-compatible compiler.
- The NTC compiler source file, `compiler/ntc.c`.
- A compatible DOS environment, such as MS-DOS 6.22, to run the generated programs.

The compiler can be built and examples can be validated using GitHub Actions.

## Your First NTC Program

Create a file named `hello.ntc`:

```ntc
PROGRAM Hello

PRINT "Hello from NTC!"
PRINT "Welcome to Neptune Code."

EXIT 0
END
```

### Understanding the program

- `PROGRAM Hello` declares the program name.
- `PRINT "Hello from NTC!"` displays a text message.
- `EXIT 0` terminates the program with exit code zero.
- `END` marks the end of the source program.

Every NTC program must begin with a `PROGRAM` declaration and end with `EXIT` followed by `END`.

## Building the Compiler

From the root of the repository, run:

```bash
mkdir -p build
gcc -std=c11 -O2 -Wall -Wextra -Werror compiler/ntc.c -o build/ntc
```

This produces the host compiler at `build/ntc`.

These commands assume GCC is installed and available in your shell.

## Compiling a Program

To compile the example:

```bash
./build/ntc examples/hello.ntc build/hello.exe
```

The compiler reads the `.ntc` source file and writes a DOS MZ executable.

The generated `.exe` is a DOS executable, not a native modern Windows application.

## Running Your Program

Transfer the generated executable into a compatible DOS environment. For example, in MS-DOS 6.22, navigate to the directory containing `HELLO.EXE` and enter:

```dos
HELLO
```

The program should display its two greeting messages.

NTC programs can also be tested in an appropriately configured DOS virtual machine.

## Exploring the Examples

The `examples/` directory contains programs demonstrating:

- Text output
- Numeric variables and assignment
- Arithmetic operations
- Text input
- Numeric input
- Comparisons
- Conditional branching
- Loops
- A small interactive dungeon adventure

Read [EXAMPLES.md](EXAMPLES.md) for the example catalog and [LANGUAGE_REFERENCE.md](LANGUAGE_REFERENCE.md) for the language syntax.

## Troubleshooting

**The compiler reports a source-line error.**

Check the line number in the error message. Confirm that instruction names, variable declarations, labels, and operands follow the language reference.

**The program does not run in Windows.**

The generated file targets 16-bit DOS. Modern 64-bit Windows does not natively execute 16-bit DOS programs. Use a compatible DOS environment instead.

**Numeric input is rejected.**

Enter a decimal integer in the supported range of 0 through 65,535. Text and empty input are not valid numeric values.

## Current Status

NTC is an evolving programming language. Consult [LIMITATIONS.md](LIMITATIONS.md) for the current target, supported data types, and known restrictions.

For a complete list of supported instructions and syntax, see [LANGUAGE_REFERENCE.md](LANGUAGE_REFERENCE.md).
