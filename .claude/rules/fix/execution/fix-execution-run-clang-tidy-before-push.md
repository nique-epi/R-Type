---
description: Run clang-tidy to a real verdict on the changed files before every push; a local tidy failure is never dismissed as an environment problem
trigger: always_on
---

# RULE: Run clang-tidy on the changed files before pushing, and read its verdict

Why: the CI `Clang-tidy` job treats warnings as errors; a local failure put down to the environment hid 12 real findings.

## Rule

1. **Before every `git push`** of C++ changes, run `clang-tidy` on the changed `.cpp` files and require exit code 0 with no `error:` on them. Capture the output in a file, read the code first (see `fix-execution-zsh-pipestatus.md`). A file the local machine cannot compile is the exception: see the last subsection.
2. **A failing local `tidy` is never "the environment" until proven.** The `cmake --build build --target tidy` target fails on macOS because the system headers are not found, and the missing headers hide or distort findings. Pass the SDK explicitly:
   ```bash
   SDK=$(xcrun --show-sdk-path)
   clang-tidy -p build --extra-arg=-isysroot --extra-arg="$SDK" \
     --extra-arg=-isystem --extra-arg="$SDK/usr/include/c++/v1" <changed .cpp files>
   ```
   Calibrate it once on a known CI failure: it must reproduce that failure's findings.
3. **Findings in files I did not touch** are reported, not hidden, and not used as the excuse to skip the files I did touch.
4. **`tidy` is part of the gates announced** with the tests and `format-check`: say which files it covered. "Not run" or "no verdict" is stated as such and the push waits for the user's decision, except for a file the local machine cannot compile (see below), which the CI lints.
5. **Header findings** (`misc-include-cleaner`, magic numbers, `misc-const-correctness`, struct alignment) apply to tests too: tests are linted like the sources.

### A new or changed header is linted with its own diagnostics shown

1. **A `.hpp` I create or edit is linted with header diagnostics enabled**: `--header-filter='.*'`. Without it clang-tidy hides every finding located in a header, and "exit 0" says nothing about the header.
2. **Prove the header was covered**: run the same command once on a deliberate fault placed *in the header's kind of code* (or on an existing header known to raise a finding) and see it reported. A calibration fault placed in the `.cpp` does not validate header coverage.
3. **A filter I wrote myself is tested before trusted**: a regular expression such as `.*/src/game/.*` does not match the relative path `src/game/Position.hpp`.
4. **The CI lints every changed `.hpp` as a main file**: its file list holds the `.cpp` and the `.hpp` the pull request changes (`git diff --name-only`), and it runs `clang-tidy -p build <file>` on each. A header finding therefore fails the CI, and some checks (`misc-include-cleaner` on an include the header itself does not use) only report when the header is the main file, never through the `.cpp` that includes it.
5. **Run the same command on each changed file**, headers included, before the push:
   ```bash
   for file in $(git diff --name-only origin/main...HEAD | grep -E '^(src|tests)/.*\.(cpp|hpp)$'); do
     clang-tidy -p build --quiet <SDK flags of point 2 of the first list> "$file" > /tmp/one.log 2>&1 || echo "$file"
   done
   ```
   A file printed is a failing file. Linting the headers through their `.cpp` with `--header-filter` does not replace it.
6. **A header borrows the compile command of a neighbouring file**: it has none of its own, so an include folder only a distant target carries is missing and clang-tidy reports `file not found`. On a push to `main` the CI lints every header, and it adds the include folders of every compiled file to header runs; do the same locally with `--extra-arg=-I<folder>` before calling such an error a defect.

### A file that does not compile for clang-tidy hides findings

1. **Count the `Error while processing` lines of the log before reading its findings.** Each one names a file clang-tidy could not compile; it was analyzed in part only, so its list of findings is a lower bound, never a verdict.
2. **Remove the compile error, then rerun until the count is 0.** On macOS, Homebrew LLVM 21 with the Xcode SDK fails on `INFINITY` in libc++ `<random>`; neutralize it with `'--extra-arg=-DINFINITY=__builtin_inff()'` (quoted for zsh) instead of dropping the files.
3. **Report the gate on the run with 0 errors**, and say which workaround it used.

### `misc-include-cleaner` judges includes against the standard library it reads

1. **The local run and the CI read different standard libraries.** The macOS run reads libc++; the CI `Clang-tidy` job configures with `cmake --preset test` on `ubuntu-latest`, whose default compiler is GCC, so it reads libstdc++. The two declare the same symbol in different internal headers: an include libc++ counts as used can be reported unused by the CI, and the other way round. A local exit code 0 does not settle the CI's verdict on includes.
2. **Before pushing, match each `#include` of a changed file against what the file names.** A standard header stays only if the file names a symbol it declares (`<cstddef>` for `std::byte` or `std::size_t`, `<cstdint>` for `std::uint8_t`); a header whose symbol the file names is included directly, never reached through another one.
3. **An include the CI reports is fixed in the file**, never put down to the environment: the CI verdict is the one that gates the merge.

### A file the local machine cannot compile

A platform file such as `FineTimerResolutionWindows.cpp` is linted only by the CI job of its platform (`Clang-tidy (Windows-only files)`). Since no local run can catch its findings, they are prevented while it is written. A file that compiles locally is never one of them: a local tidy failure on it still follows point 2.

1. **Before writing it**, read the "Return value" section of every system or C function it calls, and decide what the program does when the call fails. A result is ignored on purpose only with a cast to `void` that every check listing the function allows (point 2); otherwise the result is used. The reason goes in the doc comment of the class or function.
2. **Before pushing it**, read how the checks that police ignored results see each function it calls:
   ```bash
   clang-tidy --checks='-*,cert-err33-c,bugprone-unused-return-value' --dump-config
   ```
   Run it with the clang-tidy version the CI job prints (20 today). Match every called function against their `CheckedFunctions` patterns, and read their `AllowCastToVoid`: in clang-tidy 20, `cert-err33-c` allows the cast and `bugprone-unused-return-value` does not. A pattern without an end anchor matches more than it names: `^::time` matches `::timeBeginPeriod` and `::timeEndPeriod`.
3. **The CI is its verifier**: push, say in the report that this file is linted by the CI only, and follow the platform job until it is green, as `fix-process-follow-ci-after-opening-a-pr.md` requires. This is the exception named in points 1 and 4 of the first list. The CI runs on pull requests and on pushes to `main`, never on a feature branch pushed alone, so the user's go to publish the branch or open the pull request is still asked first.

## Example

- ❌ **Before (wrong)**: `GameMessagesTest.cpp` includes `<cstddef>` and names neither `std::byte` nor `std::size_t`; the local tidy (libc++) exits 0, the branch is pushed, and the CI `Clang-tidy` job fails on `included header cstddef is not used directly`.
- ✅ **After (right)**: before the push, the includes of each changed file are matched against the symbols it names; `<cstddef>` goes, and the CI passes.

- ❌ **Before (wrong)**: local `tidy` fails on missing system headers → "environment issue, the CI will cover it" → push → the CI reports 12 real errors.
- ✅ **After (right)**: run `clang-tidy` with the SDK flags on the four changed files → 12 findings reproduced → fix → exit 0 → push.
- ❌ **Before (wrong)**: 11 of 14 files end in `Error while processing` (`INFINITY`), the findings of that run are fixed and the files are about to be called clean.
- ✅ **After (right)**: rerun with `-DINFINITY=__builtin_inff()` → 0 files in error → 3 more `bugprone-unchecked-optional-access` findings in the new tests → fixed.
- ❌ **Before (wrong)**: `FineTimerResolutionWindows.cpp` pushed with the results of `timeBeginPeriod` and `timeEndPeriod` ignored → the Windows `Clang-tidy` job fails on `cert-err33-c`, and the code released a resolution Windows may have refused.
- ✅ **After (right)**: both "Return value" sections read → the release only follows a granted request, its result cast to `void` with the reason → `--dump-config` shows `^::time` in `cert-err33-c` and `AllowCastToVoid: true` → push, report the file as linted by the CI only, follow the Windows job until green.
