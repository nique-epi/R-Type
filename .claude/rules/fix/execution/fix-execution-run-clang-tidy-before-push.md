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

## Example

- ❌ **Before (wrong)**: local `tidy` fails on missing system headers → "environment issue, the CI will cover it" → push → the CI reports 12 real errors.
- ✅ **After (right)**: run `clang-tidy` with the SDK flags on the four changed files → 12 findings reproduced → fix → exit 0 → push.
