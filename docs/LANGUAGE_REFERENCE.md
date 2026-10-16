# NTC Language Reference

**Language:** Neptune Code (NTC)  
**Executable target:** 16-bit DOS MZ

## 1. Program Structure

Every NTC source file must contain a program declaration and a final exit instruction.

```ntc
PROGRAM Example

PRINT "Hello, world!"

EXIT 0
END
```

### `PROGRAM name`

Declares the program name.

```ntc
PROGRAM Example
```

The name must be a valid identifier supported by the compiler.

### `EXIT code`

Terminates the program with an exit code from 0 to 255.

```ntc
EXIT 0
```

An exit code of `0` conventionally indicates successful completion.

The compiler requires one `EXIT` instruction, immediately followed by `END`. No other instructions or labels may appear after `EXIT`.

### `END`

Marks the end of the source program.

## 2. Comments and Whitespace

Blank lines may be used to organize programs. The compiler supports source comments according to its implemented comment syntax. Check the compiler source if you need the exact comment markers.

Instruction keywords are written in uppercase in the examples throughout this reference.

## 3. Variables

NTC supports numeric and text variables.

### Numeric variables

Declare a numeric variable with an integer initial value:

```ntc
LET score = 100
LET lives = 3
LET zero = 0
```

Numeric values are unsigned 16-bit integers ranging from `0` to `65535`.

### Text variables

Declare a text variable with an empty string:

```ntc
LET player = ""
LET command = ""
```

Text variables use a fixed-size input buffer. The current text-input buffer supports up to 127 characters.

Text variables are used with `INPUT` and `PRINT`.

### Variable naming

Variable names must follow the identifier rules enforced by the compiler. Declare variables before using them.

## 4. Assignment

### `MOV destination, source`

Copies a numeric value into a numeric destination variable.

```ntc
LET source = 25
LET destination = 0

MOV destination, source
PRINT destination
```

Output:

```text
25
```

Assignment replaces the destination's previous value. It does not add the values together.

## 5. Arithmetic

Arithmetic instructions modify a numeric destination variable.

### `ADD destination, operand`

Adds the operand to the destination.

```ntc
LET value = 10
ADD value, 5
PRINT value
```

Result: `15`.

### `SUB destination, operand`

Subtracts the operand from the destination.

```ntc
LET value = 10
SUB value, 3
PRINT value
```

Result: `7`.

### `MUL destination, operand`

Multiplies the destination by the operand.

```ntc
LET value = 10
MUL value, 3
PRINT value
```

Result: `30`.

### `DIV destination, operand`

Divides the destination by the operand using integer division.

```ntc
LET value = 20
DIV value, 4
PRINT value
```

Result: `5`.

Division by zero is guarded against by the compiler-generated program.

Numeric arithmetic uses unsigned 16-bit values. Arithmetic results wrap modulo 65,536 where applicable.

## 6. Output

### `PRINT "text"`

Displays a string literal.

```ntc
PRINT "Welcome to NTC!"
```

The current compiler restricts characters permitted inside string literals. In particular, embedded double quotes and dollar signs are not accepted by the existing literal parser.

### `PRINT variable`

Displays the value of a variable.

For a numeric variable:

```ntc
LET score = 42
PRINT score
```

The number is displayed in decimal notation.

For a text variable:

```ntc
LET player = ""
INPUT player
PRINT player
```

The contents of the text variable are displayed.

Output operations terminate their displayed lines with a newline.

## 7. Input

### `INPUT text_variable`

Reads text from the keyboard into a previously declared text variable.

```ntc
LET player = ""

PRINT "Enter your name:"
INPUT player
PRINT player
```

The text input buffer supports up to 127 characters.

### `INPUT numeric_variable`

Reads a decimal number into a previously declared numeric variable.

```ntc
LET choice = 0

PRINT "Enter a number:"
INPUT choice
PRINT choice
```

Valid values range from `0` to `65535`. Invalid numeric input is rejected by the generated program's numeric-input validation.

Numeric input is useful for menu selections and other simple interactive programs.

## 8. Labels and Control Flow

### `LABEL name`

Defines a location within the program.

```ntc
LABEL start
PRINT "Program started."
```

Labels can be jump targets.

### `JMP name`

Transfers execution to a label.

```ntc
JMP start
```

A backward jump can be used to implement a loop.

### `CMP left, right`

Compares numeric operands. The comparison result is used by the conditional jump instructions that follow.

```ntc
LET first = 10
LET second = 10

CMP first, second
JE equal

PRINT "Different"
JMP finish

LABEL equal
PRINT "Equal"

LABEL finish
EXIT 0
END
```

`CMP` currently compares numeric values, not text strings.

### `JE name`

Jumps to a label when the most recent comparison found the operands equal.

### `JNE name`

Jumps to a label when the most recent comparison found the operands not equal.

### `JA name`

Jumps to a label when the first compared unsigned value is above the second.

### `JB name`

Jumps to a label when the first compared unsigned value is below the second.

Conditional jumps depend on the most recent comparison. Place them immediately after the intended `CMP` unless another supported instruction is deliberately used between them.

## 9. Example: Menu Selection

```ntc
PROGRAM Menu

LET choice = 0
LET one = 1

PRINT "1 - Start"
PRINT "2 - Quit"
INPUT choice

CMP choice, one
JE start_game

PRINT "You selected something other than 1."
JMP finish

LABEL start_game
PRINT "Starting..."

LABEL finish
EXIT 0
END
```

This example demonstrates numeric input and conditional branching.

## 10. Compilation

Compile an NTC program using the host compiler:

```bash
./build/ntc source.ntc output.exe
```

The output is a 16-bit DOS MZ executable.

## 11. Unsupported Features

NTC does not currently provide general-purpose string comparisons, functions, arrays, structures, or a high-level `FOR` or `WHILE` statement. Refer to [LIMITATIONS.md](LIMITATIONS.md) for the current scope and target restrictions.
