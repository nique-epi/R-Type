---
description: Zero warnings is only proven on the output of the build that actually recompiled the touched files, on the CI compilers
trigger: always_on
---

# RULE: "Zero warnings" is only proven on a build that RECOMPILES the touched files

## Rule

1. **Count warnings on THE output of the build that compiled the touched files** — the first build after the edit, never an incremental rerun that compiles nothing. Capture the full output in a file, then filter it:
   ```bash
   cmake --build build > /tmp/build.log 2>&1; echo "exit: $?"
   grep -E "warning|error" /tmp/build.log
   ```
2. **If a rerun is needed, prove it recompiled**: the output must contain the compile lines of the edited files (`Building CXX object …`). Otherwise, `cmake --build build --clean-first`.
3. **A green local build only covers its own compiler.** The CI compiles with GCC (Linux) and MSVC (Windows), with `CMAKE_COMPILE_WARNING_AS_ERROR=ON`: an MSVC warning (`size_t` → `int` conversions, `C4267`, `C4244`) does not show up with AppleClang or GCC. Announce "zero warnings" naming the compiler, and read the CI for the others.
4. The project's warning options are those of `rtype_enable_warnings()`; every new target calls it.

## Example

- ❌ **Before (wrong)**: green build → second `cmake --build build | grep -c warning` → `0` (nothing recompiled) → "zero warnings".
- ✅ **After (right)**: log of the build following the edit → empty `grep -E "warning|error"` → "zero warnings under AppleClang; GCC and MSVC checked by the PR's CI".
