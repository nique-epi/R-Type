# Rule history: zero warnings is only proven on a build that recompiles the touched files

This rule was imported from another project when R-Type started; its original incident belongs to that repository. The entry below is the first one recorded here.

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
