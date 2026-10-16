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

## Update (2026-10-08) — The restored original was not rebuilt

### Context

The asset library tests of the client screen stack work were proven red by four mutations, each applied to `AssetLibrary.cpp` or `AssetLoadingStep.cpp`, built, tested, then restored with `git checkout -- <file>`.

### Mistake

Point 4 of the red proof section was not applied: the restored original was not rebuilt by deleting its object file. The last restore happened in the same second as the mutated build, so the next build compiled nothing and four loading screen tests failed on the mutated library.

### Root cause

The section existed on disk but not in the session's context, loaded from an older branch. See `fix-process-reread-rules-after-switching-branch.md`; this rule's prescription is unchanged.

## Update (2026-10-12) — checking each commit of a branch with `git archive`

### Context

Before pushing the five commits of the game sprites branch, each intermediate commit and the merge with `main` were built and tested in an isolated copy: `git archive <commit>` extracted into the copy's tree, then `cmake --build` and `ctest` in the same build folder, one commit after the other.

### Mistake

The second and third commits each add a test, yet both reported 314 tests, like the first. The loop deleted no object file and read no `Building CXX object` line, so the stale binaries of the first commit were tested and reported as passing.

### Root cause

`git archive` gives every file the date of its commit. Those commits were a few minutes older than the objects just built for the first one, so `make` saw every source as up to date. The red proof rule already required deleting the objects and reading the build log, but the loop was written as a "build check", not as a red proof, and skipped both.

### Rule

1. `git archive <commit>` sets every extracted file to the date of that commit; a tree id gets the current time.
2. To build one commit after another in the same build folder, use `cmake --build <dir> --clean-first` for each commit, and read the number of objects compiled in its log.
3. Check the result against an expected count: a commit that adds a test must show one more test than the commit before it.

### Example

- ❌ **Before (wrong)**: `git archive 14a66e2 … | tar -x`, `cmake --build`, `ctest` → "314 passing", with the new test never compiled.
- ✅ **After (right)**: the same with `--clean-first` → every object compiled, 315 passing for the commit that adds one test.

## Update (2026-10-13) — a swapped header has several objects

Red proofs of the menu widgets, on the isolated copy. One variant set `PRESSED_TEXT_DROP` to 0 in `WidgetConstants.hpp`. The script deleted the objects of `Menu.cpp` and `MenuBuilder.cpp`, not the one of `Button.cpp`, which is the file that reads the constant; it read "5 recompiled" as proof. `Button.cpp.o` kept the old value while `MenuTest.cpp.o` got the new one: the variant showed no red test, and `APressedButtonIsPaintedPressed` turned red in the next three variants, where nothing related had changed. Rerun with every dependent object deleted (found with `grep -rln PRESSED_TEXT_DROP`), then the copy was rebuilt with `--clean-first` and ran 549/549. Points 6 and 7 were added.

## Update (2026-10-14): chained variants keep the object of the previous one

### Context

The typed messages pull request was checked with 21 red proofs on an isolated copy of the sources: for each variant a source was swapped, its object deleted, the network tests rebuilt and run, then the source restored.

### Mistake

Variant 21 (a second handler accepted) failed its own test and also the two tests of variant 20 (skipped messages not counted). The source of variant 20 had been restored less than a second before the build of variant 21, which deleted only its own object, so the faulty object of variant 20 was linked again. The whole series was rerun with every swapped object deleted before each build: variant 21 then failed its own test only, and every variant stayed red.

### Root cause

Point 2 of the red proof section deletes the object of the swapped source, and point 4 rebuilds the restored original at the end. Between two variants the restored source of the previous one was rebuilt by nobody, which neither point covers.
