# History: magic values as named constants

Rule imported from another project when R-Type started: its original incident belongs to another repository.

## Update (2026-10-03) — constants always in their own file
The plan for the client window put `windowWidth`, `windowHeight`, `windowTitle` and the frame limit in an anonymous namespace at the top of `GameWindow.cpp`, as point 2 allowed for file-local values. The user asked for them in a separate file. Point 2 now requires a dedicated constants header named after the concept (`WindowConstants.hpp`), even for a single reader.

## Update (2026-10-17) — test constants defined twice

### Context
The tests of the client animations needed a nanosecond step, the value past the last entity type, the lengths of the effects, the time of the showcase shots and a set of three frames.

### Mistake
Each new test file defined them again: `ONE_TICK` five times while `ScreenTestConstants.hpp` already held it, the same frames and animations in two files, `FIRST_VALUE_AFTER_THE_LAST_TYPE` next to `AllEntityTypes.hpp`, the effect lengths and the showcase lookup twice. The simplification review found them.

### Root cause
Each test file was written on its own, without searching the existing test headers or the other new files for the values it defined.
