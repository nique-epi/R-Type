---
description: Run clang-tidy to a real verdict on the changed files before every push; a local tidy failure is never dismissed as an environment problem
trigger: always_on
---

# RULE: Run clang-tidy on the changed files before pushing, and read its verdict

Why: the CI `Clang-tidy` job treats warnings as errors; a local failure put down to the environment hid 12 real findings.

## Rule

1. **Before every `git push`** of C++ changes, run `clang-tidy` on the changed `.cpp` files and require exit code 0 with no `error:` on them. Capture the output in a file, read the code first (see `fix-execution-zsh-pipestatus.md`).
2. **A failing local `tidy` is never "the environment" until proven.** The `cmake --build build --target tidy` target fails on macOS because the system headers are not found, and the missing headers hide or distort findings. Pass the SDK explicitly:
   ```bash
   SDK=$(xcrun --show-sdk-path)
   clang-tidy -p build --extra-arg=-isysroot --extra-arg="$SDK" \
     --extra-arg=-isystem --extra-arg="$SDK/usr/include/c++/v1" <changed .cpp files>
   ```
   Calibrate it once on a known CI failure: it must reproduce that failure's findings.
3. **Findings in files I did not touch** are reported, not hidden, and not used as the excuse to skip the files I did touch.
4. **`tidy` is part of the gates announced** with the tests and `format-check`: say which files it covered. "Not run" or "no verdict" is stated as such and the push waits for the user's decision.
5. **Header findings** (`misc-include-cleaner`, magic numbers, `misc-const-correctness`, struct alignment) apply to tests too: tests are linted like the sources.

### A new or changed header is linted with its own diagnostics shown

1. **A `.hpp` I create or edit is linted with header diagnostics enabled**: `--header-filter='.*'`. Without it clang-tidy hides every finding located in a header, and "exit 0" says nothing about the header.
2. **Prove the header was covered**: run the same command once on a deliberate fault placed *in the header's kind of code* (or on an existing header known to raise a finding) and see it reported. A calibration fault placed in the `.cpp` does not validate header coverage.
3. **A filter I wrote myself is tested before trusted**: a regular expression such as `.*/src/game/.*` does not match the relative path `src/game/Position.hpp`.
4. **Say what the CI covers**: the CI `Clang-tidy` job runs without a header filter, so header findings never fail it; they still show in the editor. Report them with their origin (new file or pre-existing) instead of announcing the headers clean.

### A file that does not compile for clang-tidy hides findings

1. **Count the `Error while processing` lines of the log before reading its findings.** Each one names a file clang-tidy could not compile; it was analyzed in part only, so its list of findings is a lower bound, never a verdict.
2. **Remove the compile error, then rerun until the count is 0.** On macOS, Homebrew LLVM 21 with the Xcode SDK fails on `INFINITY` in libc++ `<random>`; neutralize it with `'--extra-arg=-DINFINITY=__builtin_inff()'` (quoted for zsh) instead of dropping the files.
3. **Report the gate on the run with 0 errors**, and say which workaround it used.

## Example

- ❌ **Before (wrong)**: local `tidy` fails on missing system headers → "environment issue, the CI will cover it" → push → the CI reports 12 real errors.
- ✅ **After (right)**: run `clang-tidy` with the SDK flags on the four changed files → 12 findings reproduced → fix → exit 0 → push.
- ❌ **Before (wrong)**: 11 of 14 files end in `Error while processing` (`INFINITY`), the findings of that run are fixed and the files are about to be called clean.
- ✅ **After (right)**: rerun with `-DINFINITY=__builtin_inff()` → 0 files in error → 3 more `bugprone-unchecked-optional-access` findings in the new tests → fixed.
