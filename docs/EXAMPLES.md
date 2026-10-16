# Flix Examples

This document contains example programs for **Flix 0.2**, a programming language developed by NPT Software that compiles to 16-bit x86 DOS MZ executables.

Each code example is a separate program. Save an example as a source file and compile it using the Flix compiler.

## Table of Contents

1. Hello, World!
2. Numeric Variables
3. Arithmetic
4. Printing Variables
5. Text Variables and User Input
6. Labels and Unconditional Jumps
7. Comparisons and Conditional Jumps
8. Subroutines with CALL and RET
9. Combining Control Flow and Subroutines

---

## 1. Hello, World!

The simplest Flix program prints a message and exits successfully.

```text
PROGRAM HELLO

PRINT "Hello, world!"

EXIT 0
END
```

**Expected output:**

```text
Hello, world!
```

## 2. Numeric Variables

Use `LET` to declare a numeric variable and assign its initial value. Numeric values range from 0 to 65535.

```text
PROGRAM VARIABLES

LET SCORE = 100

PRINT SCORE

EXIT 0
END
```

**Expected output:**

```text
100
```

## 3. Arithmetic

Flix supports addition, subtraction, multiplication, and division through `ADD`, `SUB`, `MUL`, and `DIV`.

```text
PROGRAM ARITHMETIC

LET NUMBER = 10

ADD NUMBER, 5
PRINT NUMBER

SUB NUMBER, 3
PRINT NUMBER

MUL NUMBER, 2
PRINT NUMBER

DIV NUMBER, 4
PRINT NUMBER

EXIT 0
END
```

**Expected output:**

```text
15
12
24
6
```

## 4. Printing Variables

Use `MOV` to copy a value into another variable. Use `PRINT` to display the result.

```text
PROGRAM COPY_VALUE

LET ORIGINAL = 42
LET COPY = 0

MOV COPY, ORIGINAL

PRINT COPY

EXIT 0
END
```

**Expected output:**

```text
42
```

## 5. Text Variables and User Input

Text variables can be declared with an empty string. Use `INPUT` to read text from the user and `PRINT` to display it.

```text
PROGRAM GREETING

LET NAME = ""

PRINT "Enter your name:"
INPUT NAME

PRINT "Hello, "
PRINT NAME

EXIT 0
END
```

The program asks for a name and then prints a greeting. The exact input experience may depend on the compiler target.

## 6. Labels and Unconditional Jumps

A `LABEL` defines a named destination. `JMP` transfers execution to that label.

```text
PROGRAM JUMP_EXAMPLE

PRINT "Start"
JMP SECOND

LABEL FIRST
PRINT "First section"
JMP FINISH

LABEL SECOND
PRINT "Second section"

LABEL FINISH
PRINT "Finished"

EXIT 0
END
```

**Expected output:**

```text
Start
Second section
Finished
```

The `FIRST` label is skipped because execution jumps directly to `SECOND`.

## 7. Comparisons and Conditional Jumps

Use `CMP` to compare two values. Conditional jumps use the result of that comparison.

Available conditional jump instructions include:

| Instruction | Meaning |
|---|---|
| `JE` | Jump if equal |
| `JNE` | Jump if not equal |
| `JA` | Jump if above (unsigned) |
| `JB` | Jump if below (unsigned) |
| `JAE` | Jump if above or equal (unsigned) |
| `JBE` | Jump if below or equal (unsigned) |

Example:

```text
PROGRAM COMPARISON

LET SCORE = 100

CMP SCORE, 100
JE EQUAL

PRINT "Score is not 100"
JMP FINISH

LABEL EQUAL
PRINT "Score is 100"

LABEL FINISH
EXIT 0
END
```

**Expected output:**

```text
Score is 100
```

`CMP` performs the comparison; the conditional jump decides whether execution transfers to the specified label.

## 8. Subroutines with CALL and RET

Flix 0.2 introduces `CALL` and `RET`, allowing a program to execute a labeled subroutine and then return to the instruction after the call.

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

**Expected output:**

```text
Hello from the subroutine
Hello from the subroutine
Back in the main program
```

How it works:

1. `CALL GREETING` transfers execution to the `GREETING` label.
2. `RET` returns execution to the instruction following `CALL`.
3. The subroutine can be called more than once.
4. `JMP MAIN_END` skips over the subroutine body during normal execution.
5. `EXIT 0` terminates the program.

A subroutine called with `CALL` should return with `RET`. The main program should reach its final `EXIT` rather than fall through into a subroutine.

## 9. Combining Control Flow and Subroutines

This example combines numeric variables, comparisons, conditional jumps, and a reusable subroutine.

```text
PROGRAM CONTROL_FLOW

LET VALUE = 10

CMP VALUE, 10
JE MATCH

PRINT "Value does not match"
JMP FINISH

LABEL MATCH
CALL SHOW_MESSAGE
JMP FINISH

LABEL SHOW_MESSAGE
PRINT "Value matches"
RET

LABEL FINISH
EXIT 0
END
```

**Expected output:**

```text
Value matches
```

This demonstrates how labels, comparisons, jumps, and subroutines can be combined to organize a program.

---

## Compiler Notes

- The compiler source file remains `compiler/ntc.c`.
- Flix produces 16-bit x86 DOS MZ executables.
- `PROGRAM` begins a source program.
- `EXIT` must be the last instruction before `END`.
- Labels are defined with `LABEL name`.
- Forward references to labels can be used for jumps and subroutine calls.
- The examples use the Flix 0.2 instruction set; later releases may add or change capabilities.

Flix is developed by **Neptune Corporation**.
