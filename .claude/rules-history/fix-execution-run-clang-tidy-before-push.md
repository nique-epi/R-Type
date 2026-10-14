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

## Update (2026-10-08) — a file only Windows compiles, pushed with ignored results

### Context
The server's simulation thread asks Windows for a 1 ms timer resolution while it runs. The code lives in `FineTimerResolutionWindows.cpp`, which includes `<windows.h>`: it compiles only on Windows, and the CI lints it only in the `Clang-tidy (Windows-only files)` job. The development machine is a Mac.

### Mistake
The file called `timeBeginPeriod(1)` in a constructor and `timeEndPeriod(1)` in the destructor, ignoring both results. It was pushed with the PR described as "covered by the CI", without asking first, although point 4 required the push to wait. The Windows job failed: `cert-err33-c` on both calls. The finding was real: Microsoft's documentation says to "match each call to timeBeginPeriod with a call to timeEndPeriod"; ending only a request Windows granted is our reading of that sentence, and the code released a resolution even when Windows had refused it.

### Root cause
The "Return value" sections of the two functions were never read, and nothing prompted checking how clang-tidy sees a function before relying on a check that cannot run locally. In clang-tidy 20, `cert-err33-c` lists the pattern `^::time` without an end anchor, so it matches any function whose name starts with `time`.

### Decision
The user ruled that the CI is the verifier for such a file: the push does not wait for him, the failures are prevented while writing (return values read and handled, tidy configuration read), and the CI job is followed until green.

## Update (2026-10-10) — a file that does not compile hides findings

### Context
The game sprites work (entity sprite table, pixel canvas, tests reading the shipped sheet) was linted with clang-tidy 21 from Homebrew and the Xcode SDK, on 14 changed `.cpp` files with header diagnostics on.

### Mistake
The first run listed findings in the new tests and they were fixed. The log also held 11 `Error while processing` lines: libc++ `<random>` did not compile because `INFINITY` was undeclared. That run was nearly taken as the verdict. A second run with `-DINFINITY=__builtin_inff()` had 0 files in error and reported 3 more `bugprone-unchecked-optional-access` findings (`sprite->assetId` after `ASSERT_TRUE(sprite.has_value())`), which the CI would have failed on.

### Root cause
The rule asked for exit code 0 and no `error:` on the changed files, but said nothing about files clang-tidy could not compile. A compile error leaves clang-tidy with a partial syntax tree, so the flow-sensitive checks miss code they would otherwise analyze.

## Update (2026-10-14) — a header only fails when it is the main file

### Context
A pull request added a test fixture header, `WorldStateSenderFixture.hpp`, and the local `clang-tidy` of the changed `.cpp` files, with a header filter on the new folders, exited 0.

### Mistake
The header included a constants header it did not use. The CI `Clang-tidy` job failed with `misc-include-cleaner` ("included header ... is not used directly") on the header, announced as passing locally.

### Root cause
The rule said the CI runs without a header filter, so header findings never fail it. The CI in fact lints each changed `.hpp` as a main file. `misc-include-cleaner` judges an include against the uses inside the file being linted; through a `.cpp` that includes the header, the include looks used.

## Rule
See `.claude/rules/fix/execution/fix-execution-run-clang-tidy-before-push.md`.
