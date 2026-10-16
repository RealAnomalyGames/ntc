<p align="center">
  <img src="https://github.com/RealAnomalyGames/ntc/blob/main/logo/nptlogo.png" alt="NTC Logo">
</p>

# Flix

**Flix 0.2** is a lightweight programming language that compiles programs into 16-bit x86 DOS MZ executables. It is designed to provide a straightforward way to write small programs using a simple, instruction-oriented syntax.

Flix is developed by **NPT Software**.

## Features

Flix 0.2 includes:

- **Variables and arithmetic** — store values and perform arithmetic operations.
- **Text output** — print text and variable contents.
- **User input** — accept input from the user.
- **Comparisons** — compare values to control program flow.
- **Conditional jumps** — branch according to comparison results.
- **Labels** — define named destinations within a program.
- **Subroutines** — use `CALL` to execute a subroutine and `RET` to return.
- **Program termination** — specify an exit code with `EXIT`.
- **DOS executable generation** — produce 16-bit x86 MZ executable files.
- **Web compiler** — compile Flix programs through the browser-based compiler.

## Example

```text
PROGRAM HELLO

PRINT "Hello, world!"

EXIT 0
END
```

This program prints a greeting and exits successfully.

## Subroutines

Flix 0.2 introduces subroutine calls and returns.

```text
PROGRAM SUBROUTINE_TEST

CALL GREETING
CALL GREETING
JMP MAIN_END

LABEL GREETING
PRINT "Hello from the subroutine"
RET

LABEL MAIN_END
PRINT "Back in the main program"
EXIT 0

END
```

This example demonstrates repeated subroutine calls, returning to the calling code, and jumping over the subroutine body during normal execution.

## Compiler

The compiler is written in C and generates 16-bit x86 DOS MZ executables.

The compiler source file remains at:

`compiler/ntc.c`

The source filename is retained for continuity, even though the programming language is now called Flix.

## Web Compiler

Flix also has a browser-based compiler, allowing users to experiment with the language without setting up a local compiler environment.

See the project's repository for the web compiler and its associated files.

## Version 0.2

Flix 0.2 adds support for:

- `CALL` — call a labeled subroutine.
- `RET` — return from a subroutine.
- Near-call address resolution through the existing label and patching system.

These additions expand Flix's control-flow capabilities while preserving the existing instruction set.

## Project

Flix is a project by **Neptune Corporation**.

Visit the project's repository for compiler source code, documentation, examples, and updates.
