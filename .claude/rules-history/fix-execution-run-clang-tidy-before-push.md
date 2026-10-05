# History: run clang-tidy before pushing

## Context
A new engine time core and its tests were pushed on a feature branch. The local `cmake --build build --target tidy` failed (system headers not found on macOS, plus existing findings elsewhere).

## Mistake
The failure was classified as an environment problem without checking, and the work was committed and pushed. The CI `Clang-tidy` job failed with 12 real errors in the new files: `misc-include-cleaner` (headers used but not included directly), magic numbers in tests, a variable that could be `const`, and a struct alignment finding.

## Root cause
No rule required a clang-tidy verdict before pushing, and an unexplained failing gate was read as noise instead of being investigated. The SDK sysroot flags that make clang-tidy work locally were never tried.

## Rule
See `.claude/rules/fix/execution/fix-execution-run-clang-tidy-before-push.md`.
