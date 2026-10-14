# NTC — Neptune Code

<p align="center">
  <img src="https://github.com/RealAnomalyGames/ntc/blob/main/logo/nptlogo.png" alt="NTC Logo">
</p>


**NTC (Neptune Code)** is a general-purpose, assembly-inspired programming language developed by Neptune Corporation™.

NTC aims to make low-level programming simple and approachable while allowing developers to create games, utilities, applications, and other software.

## Build 001

The first compiler milestone targets 16-bit x86 DOS MZ executables.

### Initial features
- `PROGRAM` — Declare a program name.
- `PRINT` — Display text.
- `EXIT` — Exit with a status code.
- `END` — Mark the end of the source program.
- Automated compiler builds using GitHub Actions.

### Example

```asm
PROGRAM HelloWorld

PRINT "Hello from Neptune Code!"
EXIT 0
END
```

### Usage

```text
ntc <source.ntc> <output.exe>
```

### Development roadmap
- Build 001: DOS MZ executable generation.
- Build 002: Variables and arithmetic.
- Build 003: Branching, loops, and routines.
- Future: Windows NE applications and 32-bit/64-bit PE targets.

## Confirmed Compatibility

The following operating system has been tested and confirmed to run NTC-generated DOS MZ executables.

<p align="center">
  | Operating system | Status |
  |---|---|
  | MS-DOS 6.22 | Confirmed working |
</p>

Only tested and confirmed operating systems are listed here. Additional systems will be added as compatibility testing is completed.



**Platform note:** Build 001 produces DOS executables. Native Windows application support is planned separately and is not yet implemented.

NTC is under active development. The initial instruction set is intentionally small and will expand as the compiler matures.
