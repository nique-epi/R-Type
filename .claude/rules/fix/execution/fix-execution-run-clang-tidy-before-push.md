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
4. **Say what the CI covers**: the CI `Clang-tidy` job runs without a header filter, so header findings never fail it; they still show in the editor. Report them with their origin (new file or pre-existing) instead of announcing the headers clean.

### A file the local machine cannot compile

A platform file such as `FineTimerResolutionWindows.cpp` is linted only by the CI job of its platform (`Clang-tidy (Windows-only files)`). Since no local run can catch its findings, they are prevented while it is written. A file that compiles locally is never one of them: a local tidy failure on it still follows point 2.

1. **Before writing it**, read the "Return value" section of every system or C function it calls, and decide what the program does when the call fails. A result is ignored on purpose only with a cast to `void` that every check listing the function allows (point 2); otherwise the result is used. The reason goes in the doc comment of the class or function.
2. **Before pushing it**, read how the checks that police ignored results see each function it calls:
   ```bash
   clang-tidy --checks='-*,cert-err33-c,bugprone-unused-return-value' --dump-config
   ```
   Run it with the clang-tidy version the CI job prints (20 today). Match every called function against their `CheckedFunctions` patterns, and read their `AllowCastToVoid`: in clang-tidy 20, `cert-err33-c` allows the cast and `bugprone-unused-return-value` does not. A pattern without an end anchor matches more than it names: `^::time` matches `::timeBeginPeriod` and `::timeEndPeriod`.
3. **The CI is its verifier**: push, say in the report that this file is linted by the CI only, and follow the platform job until it is green, as `fix-process-follow-ci-after-opening-a-pr.md` requires. This is the exception named in points 1 and 4 of the first list. The CI only runs on a pull request, so the user's go to publish the branch or open the pull request is still asked first.

## Example

- ❌ **Before (wrong)**: local `tidy` fails on missing system headers → "environment issue, the CI will cover it" → push → the CI reports 12 real errors.
- ✅ **After (right)**: run `clang-tidy` with the SDK flags on the four changed files → 12 findings reproduced → fix → exit 0 → push.
- ❌ **Before (wrong)**: `FineTimerResolutionWindows.cpp` pushed with the results of `timeBeginPeriod` and `timeEndPeriod` ignored → the Windows `Clang-tidy` job fails on `cert-err33-c`, and the code released a resolution Windows may have refused.
- ✅ **After (right)**: both "Return value" sections read → the release only follows a granted request, its result cast to `void` with the reason → `--dump-config` shows `^::time` in `cert-err33-c` and `AllowCastToVoid: true` → push, report the file as linted by the CI only, follow the Windows job until green.
