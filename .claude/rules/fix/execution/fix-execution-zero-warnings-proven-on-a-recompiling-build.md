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

### MSVC warns inside the standard library, where Clang and GCC stay silent

1. **A narrowing conversion done by a standard algorithm on my behalf is mine.** `std::tolower` and `std::toupper` return an `int`; `std::ranges::transform` or `std::transform` storing it in a `char` makes MSVC raise `C4244` from inside `<algorithm>`, and the Windows job fails. Cast at the source: `static_cast<char>(std::tolower(character))`.
2. **Before pushing code that hands a lambda to a standard algorithm, or code copied from a project that was never built with MSVC**, run a proxy with Clang, which hides warnings from system headers by default:
   ```bash
   clang++ <flags of the target> -fsyntax-only -ferror-limit=0 -Wsystem-headers -Wconversion -Wsign-conversion
   ```
   Keep only the warnings whose text names my own code (a lambda at one of my files, or a line of my files): without that filter, the internals of libc++ bury them.
3. **Calibrate the proxy on a known case first**, as `fix-reasoning-validate-the-instrument-before-reporting-a-count.md` requires: it must show the conversion that already failed on the CI, and stop showing it once fixed. The proxy does not replace the Windows job; it only narrows what is left to find there.
4. **A failing MSVC job hides the files that depend on the failed target**: MSBuild stops building them, so the log lists only what was compiled before the failure. Fix the first error, then expect that the next push may reveal more.

## Example

- ❌ **Before (wrong)**: green build → second `cmake --build build | grep -c warning` → `0` (nothing recompiled) → "zero warnings".
- ✅ **After (right)**: log of the build following the edit → empty `grep -E "warning|error"` → "zero warnings under AppleClang; GCC and MSVC checked by the PR's CI".
