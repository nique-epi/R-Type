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

## Update (2026-10-14): an include libc++ counts as used, the CI does not

### Context
The typed messages pull request was linted locally with clang-tidy 20 from Homebrew and the macOS SDK, on every changed `.cpp` with header diagnostics on: exit 0, no finding.

### Mistake
The CI `Clang-tidy` job failed on one finding: `tests/network/Protocol/GameMessagesTest.cpp` included `<cstddef>` without naming `std::byte` or `std::size_t`. The local run had not reported it, and the branch had been pushed on that run alone.

### Root cause
`misc-include-cleaner` decides whether an include is used from the declarations of the standard library it reads. The local run reads libc++, the CI reads the libstdc++ of GCC on Ubuntu; they spread the same symbols over different internal headers. The rule asked for a clean local run, which cannot tell what the CI's standard library will say.

## Update (2026-10-15) — a finding only the CI's clang-tidy reports

### Context
Pull request #69 (title screen and menu widgets). Every changed `.cpp` and `.hpp` was linted locally as a main file, with the SDK flags and `-DINFINITY=__builtin_inff()`, before the push.

### Mistake
The CI `Clang-tidy` job failed on one finding the local run never reported: `tests/client/Screens/RecordingNavigator.hpp:19`, "constructor does not initialize these fields: request" (`cppcoreguidelines-pro-type-member-init`) on `struct NavigatorCall { NavigatorRequest request; std::optional<ScreenId> screen; }`. Running the local clang-tidy 21.1.8 on that header alone again gave exit 0 and no finding.

### Root cause
The local toolchain (clang-tidy 21, libc++) differs from the CI's (clang-tidy 20, libstdc++); which of the two makes the difference was not measured. Linting locally cannot prove a member-init check silent on the CI.

### What was done
`request{}` given as default member initializer; the rule now asks for a default initializer on every data member of a new struct or class.

## Rule
See `.claude/rules/fix/execution/fix-execution-run-clang-tidy-before-push.md`.

## Update (2026-10-14) — headers are linted by the CI
The point saying the CI never lints headers was wrong since #64: `main` has failed `Clang-tidy` since `d15aff0`, 35 errors in headers no pull request had touched since. Point 4 was rewritten and point 5 added: a header lint needs the include folders of every compiled file.

## Update (2026-10-15) — a configuration that does not parse checks nothing

### Context
Connection screen branch, started from `main` after the merge of the title screen pull request (#69) and of "make Clang-tidy pass on main again" (#72). Both had added `cppcoreguidelines-special-member-functions.AllowSoleDefaultDtor` and `hicpp-special-member-functions.AllowSoleDefaultDtor` to `.clang-tidy`; the merge kept both copies.

### Mistake
The 43 changed files were linted with Homebrew clang-tidy 21: exit 0 on all but one header, which needed absolute `-I` folders; "clang-tidy clean" was written in `.context/context.md`. The header calibration that followed reported nothing for a planted unused `#include <vector>`. Every log began with `.clang-tidy:30:3: error: duplicated mapping key` and `Error parsing .clang-tidy: Invalid argument`: clang-tidy had run its default checks only. The CI `Clang-tidy` job on `main` (clang-tidy 20) printed the same error 1278 times and passed.

### Root cause
The rule asked for exit 0 and for a calibration, but not for the calibration to come before the verdict, nor for the log to be read for a configuration error that does not change the exit code.

### What was done
A deduplicated copy of the configuration passed with `--config-file` reported the planted fault; the 43 files were linted again with it. The broken configuration on `main` was reported to the user, its fix left to a separate branch.


## Update (23 October 2026) — forward declarations to silence a borrowed command

While applying Yannis Lorang's review of pull request #103, `GameScreen::Parts` moved to `src/client/Screens/GameScreenParts.hpp`. Linted alone, the header borrowed the compile command of a file of `Screens/` and stopped on `'IEntityMessageSource.hpp' file not found`. Instead of rerunning it with the include folders of every compiled file (point 6 of the first list, and what the CI's `Clang-tidy` step does), the includes were replaced by forward declarations. `GameScreen.cpp`, which calls members of `IServerConnection`, then failed to compile; given the real include, `misc-include-cleaner` called it unused, because it took the forward declaration as the provider. The includes were put back, the header linted with the CI's include folders (exit 0), and the include of `GameScreen.cpp` dropped. The lint script of the session now passes those folders to every header.

## Update (24 October 2026) — Code moved out of a file takes its uses with it

The answer to a review moved the constants of the test files into `…TestConstants.hpp` headers. `MissionBannerTest.cpp` kept `#include <cstddef>`, whose only use, a `std::size_t` constant, had moved to `MissionBannerTestConstants.hpp`. The local clang-tidy (libc++) passed it; the CI's `misc-include-cleaner` (libstdc++) failed on it. Point 2 of the subsection already asked to match each include against what the file names: it was not applied to the files the move emptied. New point 4: after a move, match the standard includes left behind with a script over every touched file.
