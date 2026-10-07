# Rule history: zero warnings is only proven on a build that recompiles the touched files

This rule was imported from another project when R-Type started; its original incident belongs to that repository. The entries below are the ones recorded here.

## Update (2026-10-06) — MSVC warns inside the standard library

### Context

The logging pull request copied a logger from another project that had only ever been built with GCC and Clang. The local build (AppleClang) and the Linux job (GCC) were green, with zero warnings, and the format and clang-tidy jobs passed.

### Mistake

The Windows job failed: `warning C4244: conversion from 'int' to 'char'`, treated as an error, reported at `<algorithm>(4414)` and traced to a lambda in `Logger.cpp`. The lambda passed to `std::ranges::transform` returned the `int` of `std::tolower`, which the algorithm stored in a `char`. The earlier risk statement in the pull request ("MSVC only by the CI") was true, but nothing had been done to narrow it before pushing.

### Root cause

- Clang and GCC hide warnings that originate in system headers, and this one is raised by the instantiation of a standard template, so no local compiler showed it.
- Only the files compiled before the failure appear in the MSVC log: everything depending on the failed library (the launch options, both entry points, all the logging tests) was never seen by MSVC, so the first error did not say whether others were waiting.

### Rule

See the section "MSVC warns inside the standard library, where Clang and GCC stay silent" of the core rule: cast conversions at the source, run a Clang proxy with `-Wsystem-headers -Wconversion -Wsign-conversion` filtered on my own code, calibrate it on the known failure, and expect a failing MSVC job to hide further errors.

### Example

- ❌ **Before (wrong)**: `std::ranges::transform(text, text.begin(), [](unsigned char character) { return std::tolower(character); });` — green on AppleClang and GCC, `C4244` on MSVC.
- ✅ **After (right)**: `return static_cast<char>(std::tolower(character));`, and a Clang run with `-Wsystem-headers -Wconversion` that no longer lists the lambda.

## Update (2026-10-06) — A red proof runs on a binary rebuilt from the faulty source

### Context

The movement system came with four tests, each to be seen failing on a deliberately faulty version of `MovementSystem.cpp` before being kept. A shell loop copied each faulty version over the source, ran `cmake --build build --target game_tests`, then ran the tests.

### Mistake

The fourth variant produced exactly the same failures as the third. Its build log showed no `Building CXX object` line and no link: nothing had been rebuilt, and the tests ran the binary of the previous variant. Read without the log, the run would have reported that the test guarding entities without a velocity does not catch its defect, and that the zero-time test catches a defect it was not given.

### Root cause

The loop relied on `make` noticing the copied file. The copy happened less than a second after the previous build, and `make` compares timestamps to the second, so the object file looked up to date. The results were read without first checking, in each build log, that the swapped file had been compiled.

### Rule

1. Swapping a source file and rebuilding in a loop is not enough: `make` compares timestamps to the second, so a file copied less than a second after the previous build is seen as up to date and the old binary runs.
2. Before each build of a variant, delete the object file of the swapped source (`rm -f build/<path>/CMakeFiles/<target>.dir/<File>.cpp.o`), never touch the sources to force it.
3. Read the build log of each variant before its test result: it must contain the `Building CXX object` line of the swapped file. A variant without that line is not a result; rerun it.
4. Rebuild the restored original the same way and run its tests green: the last binary left in `build/` must be the real code.

### Example

- ❌ **Before (wrong)**: `for fault in …; do cp "$fault.cpp" src/…/MovementSystem.cpp; cmake --build build --target game_tests; build/tests/game_tests; done` → the fourth run tests the third binary.
- ✅ **After (right)**: the same loop with `rm -f <object file>` before each build and `grep -c "Building CXX object …MovementSystem.cpp.o"` equal to 1 in each log before reading the tests.
