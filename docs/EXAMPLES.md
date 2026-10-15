# NTC Examples

This directory contains example programs for Neptune Code (NTC). They demonstrate the language features available in the current compiler.

All examples target 16-bit DOS MZ executables.

## Example Catalog

| File | Description |
|---|---|
| `hello.ntc` | Displays a greeting using text output. |
| `variables.ntc` | Declares numeric variables and demonstrates assignment with `MOV`. |
| `arithmetic.ntc` | Demonstrates addition, subtraction, multiplication, and division. |
| `text_input.ntc` | Accepts text from the keyboard and displays it. |
| `numeric_input.ntc` | Accepts a decimal integer and displays the result. |
| `comparison.ntc` | Compares numeric values and branches based on equality. |
| `branching.ntc` | Demonstrates conditional jumps and labels. |
| `countdown.ntc` | Uses a backward jump to implement a countdown loop. |
| `skullgeon_demo.ntc` | Demonstrates numeric menu input and multiple narrative paths in a small dungeon scenario. |

## Compiling Examples

Build the compiler from the repository root:

```bash
mkdir -p build
gcc -std=c11 -O2 -Wall -Wextra -Werror compiler/ntc.c -o build/ntc
```

Compile an individual example:

```bash
./build/ntc examples/hello.ntc build/hello.exe
```

Replace `hello` with the example's filename to compile another program.

To compile all examples:

```bash
mkdir -p build/examples

for source in examples/*.ntc; do
    name="$(basename "$source" .ntc)"
    ./build/ntc "$source" "build/examples/$name.exe" || exit 1
done
```

The resulting `.exe` files are DOS MZ executables, not native modern Windows applications.

## What to Learn from Each Example

### Hello

Start here to learn `PROGRAM`, `PRINT`, `EXIT`, and `END`.

### Variables

Learn how `LET` declares numeric variables and how `MOV` copies values.

### Arithmetic

Learn how arithmetic instructions modify their destination variables.

### Text Input

Learn how to declare a text variable, read keyboard input, and display the result.

### Numeric Input

Learn how to accept numeric input for menu choices and simple calculations.

### Comparison

Learn how `CMP` and `JE` can make a program take different paths.

### Branching

Study how labels, conditional jumps, and `JMP` combine to form a decision tree.

### Countdown

Learn how to implement repetition using a label and a backward jump.

### The Skullgeon Demo

See how numeric input and branching can be combined into a small interactive dungeon scenario. This is a demonstration, not the full game.

## Testing

Compilation confirms that the compiler accepts a source file and generates an output executable. It does not, by itself, guarantee correct runtime behavior.

Test interactive examples in a compatible DOS environment. In particular, check numeric input, branching, and the countdown loop before relying on them in a larger project.

For syntax details, consult [LANGUAGE_REFERENCE.md](LANGUAGE_REFERENCE.md). For known restrictions, consult [LIMITATIONS.md](LIMITATIONS.md).
