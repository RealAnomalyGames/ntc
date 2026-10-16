# Flix Language Reference

**Version:** 0.2  
**Language developer:** Neptune Corporation  
**Engine:** NPT Software  
**Compiler source:** `compiler/ntc.c`

Flix is an instruction-oriented programming language that compiles programs into 16-bit x86 DOS MZ executables. This document describes the language syntax and instructions available in Flix 0.2.

## 1. Program Structure

A Flix program begins with `PROGRAM` and ends with `END`. The final instruction before `END` must be `EXIT`.

```text
PROGRAM program_name

PRINT "Hello, world!"

EXIT 0
END
```

### Structural keywords

| Keyword | Purpose |
|---|---|
| `PROGRAM name` | Declares the program's name. |
| `END` | Marks the end of the source program. |
| `EXIT code` | Terminates the program with an exit code from 0 to 255. |

**Rule:** `EXIT` must be the last instruction before `END`. Do not place labels, subroutines, or other instructions between `EXIT` and `END`.

## 2. Syntax Conventions

Examples in this document use these conventions:

- `name` represents a valid identifier.
- `value` represents a numeric value or a supported operand.
- `label` represents a label name.
- Quoted text, such as `"Hello"`, represents a string literal.
- Keywords and instruction names are written in uppercase for consistency.

Use commas to separate operands where shown. Follow the exact operand structure of each instruction.

## 3. Variables

### `LET`

Declares a variable and assigns its initial value.

Numeric syntax:

```text
LET name = integer
```

Example:

```text
LET SCORE = 100
```

Numeric values range from 0 to 65535.

Text syntax:

```text
LET name = ""
```

Example:

```text
LET NAME = ""
```

Text variables are initialized with an empty string.

### `MOV`

Copies a value from a source operand into a destination variable.

```text
MOV destination, source
```

Example:

```text
LET SOURCE = 42
LET TARGET = 0

MOV TARGET, SOURCE
PRINT TARGET
```

The example prints `42`.

## 4. Arithmetic Instructions

Arithmetic instructions operate on numeric variables.

| Instruction | Syntax | Purpose |
|---|---|---|
| `ADD` | `ADD destination, operand` | Adds an operand to the destination. |
| `SUB` | `SUB destination, operand` | Subtracts an operand from the destination. |
| `MUL` | `MUL destination, operand` | Multiplies the destination by an operand. |
| `DIV` | `DIV destination, operand` | Divides the destination by an operand. |

Example:

```text
PROGRAM ARITHMETIC

LET VALUE = 10

ADD VALUE, 5
SUB VALUE, 2
MUL VALUE, 3
DIV VALUE, 3

PRINT VALUE

EXIT 0
END
```

The final value is `13`.

Division by zero should be avoided. Consult the compiler's current implementation and tests for precise arithmetic edge-case behavior.

## 5. Output Instructions

### `PRINT` with a string literal

```text
PRINT "text"
```

Example:

```text
PRINT "Hello, world!"
```

### `PRINT` with a variable

```text
PRINT variable
```

Example:

```text
LET SCORE = 100
PRINT SCORE
```

### Printing text alongside a text variable

Flix supports text variables and text-variable output functionality. For example:

```text
LET NAME = ""

INPUT NAME
PRINT "Hello, "
PRINT NAME
```

This example reads text into `NAME` and prints a greeting.

## 6. Input Instructions

### `INPUT`

Reads user input into a text variable.

```text
INPUT variable
```

Example:

```text
PROGRAM USER_INPUT

LET NAME = ""

PRINT "Enter your name:"
INPUT NAME

PRINT NAME

EXIT 0
END
```

The variable should be declared as a text variable before it is used with `INPUT`.

## 7. Labels

A label defines a named destination in a program.

```text
LABEL name
```

Example:

```text
LABEL DESTINATION
PRINT "Reached destination"
```

Labels can be referenced before their definitions, allowing forward references. Each label used by a jump or subroutine call must be defined.

## 8. Unconditional Jumps

### `JMP`

Transfers execution to a label.

```text
JMP label
```

Example:

```text
PROGRAM JUMP_EXAMPLE

PRINT "Start"
JMP FINISH

PRINT "Skipped"

LABEL FINISH
PRINT "Finished"

EXIT 0
END
```

Expected output:

```text
Start
Finished
```

## 9. Comparisons

### `CMP`

Compares two operands for use by conditional jumps.

```text
CMP left, right
```

Operands can be supported numeric literals or declared numeric variables.

Example:

```text
LET SCORE = 100
CMP SCORE, 100
JE MATCH
```

`CMP` establishes the comparison result; a following conditional jump determines whether execution transfers to its target label.

## 10. Conditional Jumps

Flix 0.2 supports the following conditional jump instructions.

| Instruction | Condition |
|---|---|
| `JE label` | Jump if equal. |
| `JNE label` | Jump if not equal. |
| `JA label` | Jump if above, using unsigned comparison. |
| `JB label` | Jump if below, using unsigned comparison. |
| `JAE label` | Jump if above or equal, using unsigned comparison. |
| `JBE label` | Jump if below or equal, using unsigned comparison. |

Example:

```text
PROGRAM COMPARISON

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

The `JA`, `JB`, `JAE`, and `JBE` instructions use **unsigned** comparisons. They should not be interpreted as signed-integer comparisons.

## 11. Subroutines

Flix 0.2 introduces `CALL` and `RET` for subroutine execution.

### `CALL`

Transfers execution to a label while preserving the return address.

```text
CALL label
```

### `RET`

Returns to the instruction immediately following the most recent corresponding `CALL`.

```text
RET
```

### Complete example

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

Expected output:

```text
Hello from the subroutine
Hello from the subroutine
Back in the main program
```

A subroutine can be called repeatedly. Use `RET` to return from a called subroutine, and use a jump to prevent normal execution from falling through into the subroutine body.

A `RET` without a valid corresponding call is not a normal program-termination mechanism. Use `EXIT` to terminate the program.

## 12. Complete Instruction Summary

| Instruction | Syntax | Description |
|---|---|---|
| `PROGRAM` | `PROGRAM name` | Begins a program declaration. |
| `LET` | `LET name = value` | Declares and initializes a variable. |
| `MOV` | `MOV destination, source` | Copies a value. |
| `ADD` | `ADD destination, operand` | Adds to a numeric variable. |
| `SUB` | `SUB destination, operand` | Subtracts from a numeric variable. |
| `MUL` | `MUL destination, operand` | Multiplies a numeric variable. |
| `DIV` | `DIV destination, operand` | Divides a numeric variable. |
| `PRINT` | `PRINT "text"` or `PRINT variable` | Outputs text or a variable. |
| `INPUT` | `INPUT variable` | Reads text input. |
| `LABEL` | `LABEL name` | Defines a control-flow destination. |
| `JMP` | `JMP label` | Jumps unconditionally. |
| `CMP` | `CMP left, right` | Compares two operands. |
| `JE` | `JE label` | Jumps if equal. |
| `JNE` | `JNE label` | Jumps if not equal. |
| `JA` | `JA label` | Jumps if unsigned above. |
| `JB` | `JB label` | Jumps if unsigned below. |
| `JAE` | `JAE label` | Jumps if unsigned above or equal. |
| `JBE` | `JBE label` | Jumps if unsigned below or equal. |
| `CALL` | `CALL label` | Calls a subroutine. |
| `RET` | `RET` | Returns from a subroutine. |
| `EXIT` | `EXIT code` | Terminates with an exit code. |
| `END` | `END` | Marks the end of the program. |

## 13. Executable Format

Flix 0.2 generates **16-bit x86 DOS MZ executables**. These programs are intended to run in a compatible DOS environment or emulator.

The compiler itself is written in C. Its source file remains named `compiler/ntc.c`; the filename is retained for continuity and does not change the public name of the language.

## 14. Version 0.2

Flix 0.2 includes numeric and text variables, arithmetic, output, input, comparisons, labels, conditional and unconditional jumps, and subroutine calls and returns.

The `CALL` and `RET` instructions extend the language's control-flow capabilities by allowing programs to reuse a labeled block of code and return to the caller.

## Project Credits

**Flix** is developed by **Neptune Corporation**.

**NPT Software** is the engine associated with the project.
