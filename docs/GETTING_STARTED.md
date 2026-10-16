# Getting Started with Flix

Welcome to **Flix**, a programming language developed by **Neptune Corporation**. Flix compiles programs into 16-bit x86 DOS MZ executables and provides a simple, instruction-oriented way to write programs.

Flix 0.2 introduces subroutine support through the `CALL` and `RET` instructions.

## 1. What You Need

To get started, you need:

- The Flix compiler.
- A text editor for writing source files.
- A C compiler if you intend to build the compiler from source.
- A DOS environment or emulator, such as DOSBox, to run the generated DOS executables.

You can also use the browser-based compiler if it is available through the project.

## 2. Create Your First Program

Create a new file named `hello.flix` and enter the following code:

```text
PROGRAM HELLO

PRINT "Hello, world!"

EXIT 0
END
```

### Understanding the program

- `PROGRAM HELLO` declares the program's name.
- `PRINT "Hello, world!"` displays a message.
- `EXIT 0` terminates the program with exit code zero.
- `END` marks the end of the source file.

**Important:** `EXIT` must be the last instruction before `END`.

## 3. Compile Your Program

The compiler source file remains named `compiler/ntc.c`. The filename is retained even though the programming language is now called Flix.

If you have built the compiler executable, invoke it with the source path followed by the desired output path:

```text
ntc hello.flix hello.exe
```

This command illustrates the compiler's argument format:

```text
ntc <source-file> <output-file>
```

The compiler produces a 16-bit x86 DOS MZ executable. The precise build command depends on your environment and the project's current build configuration.

## 4. Run Your Program

Run the generated `hello.exe` in a compatible DOS environment.

Expected output:

```text
Hello, world!
```

If you are using a browser-based compiler, follow its on-screen instructions to compile and run your program.

## 5. Use Variables and Arithmetic

Flix supports numeric variables and basic arithmetic operations.

```text
PROGRAM CALCULATOR

LET VALUE = 10

ADD VALUE, 5
SUB VALUE, 2

PRINT VALUE

EXIT 0
END
```

Expected output:

```text
13
```

The program declares a variable, adds five, subtracts two, and prints the result.

## 6. Use Labels and Jumps

Labels let you name destinations within your program. Use `JMP` to transfer execution to a label.

```text
PROGRAM JUMP_EXAMPLE

PRINT "Before the jump"
JMP DESTINATION

PRINT "This message is skipped"

LABEL DESTINATION
PRINT "After the jump"

EXIT 0
END
```

Expected output:

```text
Before the jump
After the jump
```

## 7. Call a Subroutine

Flix 0.2 supports subroutines using `CALL` and `RET`.

```text
PROGRAM SUBROUTINE_EXAMPLE

CALL GREETING
PRINT "Returned to the main program"
JMP FINISH

LABEL GREETING
PRINT "Hello from a subroutine"
RET

LABEL FINISH
EXIT 0
END
```

Expected output:

```text
Hello from a subroutine
Returned to the main program
```

### How it works

1. `CALL GREETING` transfers execution to the `GREETING` label.
2. The subroutine prints its message.
3. `RET` returns execution to the instruction following `CALL`.
4. The main program continues and then jumps over the subroutine body.
5. The program reaches its final `EXIT`.

Always use `RET` to return from a subroutine invoked with `CALL`. Use an appropriate jump when ordinary execution needs to skip over a subroutine's definition.

## 8. Compare Values

Use `CMP` with a conditional jump to make a decision.

```text
PROGRAM CHECK_VALUE

LET VALUE = 25

CMP VALUE, 25
JE MATCH

PRINT "No match"
JMP FINISH

LABEL MATCH
PRINT "Match found"

LABEL FINISH
EXIT 0
END
```

Expected output:

```text
Match found
```

Flix 0.2 supports conditional jumps including `JE`, `JNE`, `JA`, `JB`, `JAE`, and `JBE`.

## 9. Where to Go Next

After completing these examples:

- Read `EXAMPLES.md` for more sample programs.
- Consult the project documentation for the supported instructions and syntax.
- Experiment with variables, arithmetic, comparisons, labels, and subroutines.
- Test generated executables in a compatible DOS environment.

## Project Information

**Language:** Flix  
**Language developer:** Neptune Corporation  
**Engine:** NPT Software  
**Version:** 0.2  
**Compiler source:** `compiler/ntc.c`  
**Target format:** 16-bit x86 DOS MZ executable

The compiler source filename remains unchanged for continuity. Renaming the language does not require changing the compiler's filename or rewriting its implementation.
