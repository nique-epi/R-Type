# History: run clang-tidy before pushing

## Context
A new engine time core and its tests were pushed on a feature branch. The local `cmake --build build --target tidy` failed (system headers not found on macOS, plus existing findings elsewhere).

## Mistake
The failure was classified as an environment problem without checking, and the work was committed and pushed. The CI `Clang-tidy` job failed with 12 real errors in the new files: `misc-include-cleaner` (headers used but not included directly), magic numbers in tests, a variable that could be `const`, and a struct alignment finding.

## Root cause
No rule required a clang-tidy verdict before pushing, and an unexplained failing gate was read as noise instead of being investigated. The SDK sysroot flags that make clang-tidy work locally were never tried.

## Update (2026-10-05) — header findings hidden by a filter that matched nothing

### Context
Four new headers (playfield constants and three plain structs) were added to the game library. clang-tidy was run on the test including them and on a scratch file, with `--header-filter='.*/src/game/.*'` and relative include paths.

### Mistake
clang-tidy was announced clean on the new files. The user's editor showed `altera-struct-pack-align` on the three structs. With `--header-filter='.*'` the same run reports that finding, plus `portability-avoid-pragma-once`, `llvm-header-guard` and `misc-non-private-member-variables-in-classes`, on the new headers and on the pre-existing `Entity.hpp` alike.

### Root cause
The filter required a `/` before `src`, so it never matched the relative header paths and every header finding stayed suppressed. The calibration fault (a positional initializer) was placed in a `.cpp`, so it proved the checks ran on the main file only, not on headers.

## Rule
See `.claude/rules/fix/execution/fix-execution-run-clang-tidy-before-push.md`.
