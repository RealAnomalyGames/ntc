# Flix — Limitations

Flix is a programming language developed by Neptune Corporation. Version 0.2 focuses on compiling programs into 16-bit x86 DOS MZ executables.

Although Flix supports basic programming constructs, it is still under development and has several limitations.

## Platform Limitations

- **16-bit DOS executables:** The compiler currently targets 16-bit x86 DOS MZ executables. It does not natively produce 32-bit or 64-bit Windows applications.
- **DOS environment:** Compiled programs are intended to run in a compatible DOS environment, such as MS-DOS, FreeDOS, or a suitable emulator.
- **Limited operating system integration:** Flix does not currently provide extensive access to modern operating system features or APIs.

## Language Limitations

- **Basic language features:** Flix focuses on fundamental programming instructions, arithmetic, variables, input/output, labels, conditional jumps, and subroutines.
- **Limited data types:** Variable and data-type support is basic and may not cover the needs of more complex programs.
- **Limited control structures:** Program flow is primarily managed through labels, jumps, comparisons, and subroutine calls rather than higher-level constructs.
- **No advanced language features:** Features such as complex data structures, object-oriented programming, and a comprehensive standard library are not currently part of the language.

## Compiler Limitations

- **Target-specific output:** The compiler is designed around its current 16-bit DOS executable target.
- **Limited diagnostics:** Error messages and debugging support may be less comprehensive than those of mature programming languages.
- **Ongoing development:** Compiler behavior, language features, and documentation may change in future releases.

## Web Compiler

A browser-based compiler is also available. Its capabilities and execution environment may differ from those of the native compiler, and it should not be assumed to provide identical behavior in every situation.

## Future Development

These limitations describe the current state of Flix and are not necessarily permanent. Future versions may expand the language, improve diagnostics, add capabilities, and support additional targets.

For current syntax and supported features, consult the language reference and examples included with the project.

---

**Language:** Flix  
**Developer:** Neptune Corporation  
**Current version:** 0.2  
**Executable target:** 16-bit x86 DOS MZ  
