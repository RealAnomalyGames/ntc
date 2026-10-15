<p align="center">
  <img src="https://github.com/RealAnomalyGames/ntc/blob/main/logo/nptlogo.png" alt="NTC Logo">
</p>

# NTC — Neptune Code

**NTC (Neptune Code)** is a lightweight, assembly-inspired programming language developed by Neptune Corporation. It is designed to provide a straightforward way to write programs and compile them into executable files for DOS.

The project includes the NTC compiler, a browser-based compiler interface, and NPT Software, a lightweight environment for writing NTC programs and exporting DOS executables.

## Project Overview

- **Language:** Neptune Code (NTC)
- **Developer:** Neptune Corporation
- **Compiler implementation:** C
- **Compiler target:** 16-bit x86 DOS MZ executables
- **Web technologies:** HTML, CSS, and JavaScript for the browser-based tools
- **Development philosophy:** Simple, lightweight, and accessible

## Tools

### NTC Web Compiler

The NTC Web Compiler provides a browser-based interface for compiling NTC source code.

- Compile NTC source files in the browser.
- Generate DOS MZ executable files.
- Experiment with the language without setting up the compiler locally.

**[Open the NTC Web Compiler](https://realanomalygames.github.io/ntc/compiler/)**

### NPT Software 0.1

NPT Software is a lightweight NTC development tool built around the existing NTC compiler.

Features include:

- An integrated NTC source editor.
- Create, open, and save `.NTC` source files.
- Compile NTC programs into DOS `.EXE` files.
- Export compiled executables for use outside the browser.
- Compiler output and error reporting.

NPT Software does not run DOS executables inside the browser. Compiled programs can be exported and run separately in a compatible DOS environment.

**[Open NPT Software](https://realanomalygames.github.io/ntc/npt-software/)**

## Getting Started

### Your First NTC Program

Create a file named `HELLO.NTC` and enter the following code:

```ntc
PROGRAM
PRINT "HELLO FROM NTC!"
EXIT 0
END
```

### Compile and Run

1. Open the NTC Web Compiler or NPT Software.
2. Enter the program into the source editor.
3. Compile the source code into a DOS executable.
4. Export the resulting `.EXE` file.
5. Run the executable in a compatible DOS environment.

The compiler produces DOS MZ executables. Successful compilation does not replace testing the resulting program in its intended environment.

## Language Features

NTC 0.1 supports the following instructions:

| Instruction | Purpose |
|---|---|
| `PROGRAM` | Begins a program |
| `LET` | Defines a variable with a value |
| `MOV` | Moves a value between variables |
| `ADD` | Addition |
| `SUB` | Subtraction |
| `MUL` | Multiplication |
| `DIV` | Division |
| `PRINT` | Displays text or a variable |
| `INPUT` | Accepts input |
| `LABEL` | Defines a jump target |
| `JMP` | Jumps to a label |
| `CMP` | Compares values |
| `JE` | Jumps when values are equal |
| `JNE` | Jumps when values are not equal |
| `JA` | Conditional jump for an above comparison |
| `JB` | Conditional jump for a below comparison |
| `EXIT` | Ends the program with an exit code |
| `END` | Marks the end of the source program |

Consult the language documentation in this repository for the exact syntax, restrictions, and usage examples.

## Compatibility

NTC generates 16-bit x86 DOS MZ executables. The compiler and its output should be distinguished from the environment used to run the resulting programs.

NTC-generated executables have been tested successfully in **MS-DOS 6.22**.

Other DOS versions and DOS-compatible environments may work, but compatibility should be verified through testing.

## Repository Structure

```text
ntc/
├── web/
│   ├── compiler/       # NTC Web Compiler
│   └── npt-software/   # NPT Software
├── README.md
└── ...
```

This is a simplified overview; additional source files, documentation, and build configuration are maintained in the repository.

## Development

NTC is developed and maintained by Neptune Corporation. Development takes place alongside the project's compiler, documentation, and browser-based tools.

When reporting a problem, include the NTC source code that reproduces it, the compiler output, and the environment in which the resulting executable was tested. Do not include private information in bug reports.

## Project Status

NTC 0.1 and NPT Software 0.1 provide the initial programming and compilation workflow.

The current focus is on maintaining reliable compilation, documenting the language, and improving the development experience.

## Links

- [NTC Web Compiler](https://realanomalygames.github.io/ntc/compiler/)
- [NPT Software](https://realanomalygames.github.io/ntc/npt-software/)
- [NTC GitHub Repository](https://github.com/realanomalygames/ntc)

---

**NTC — Neptune Code**

Developed by **Neptune Corporation™**.
