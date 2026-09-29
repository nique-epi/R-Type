---
description: In zsh, an unquoted variable or $(...) is not split into arguments — pass lists as arrays or through find -print0 | xargs -0
trigger: always_on
---

# RULE: In zsh, unquoted `$var` and `$(...)` are NOT split into arguments

## Rule

1. **Never pass a list of files or options through `cmd $var` or an unquoted `cmd $(...)`**: zsh does not split like bash, the command gets **one** giant argument.
2. **List of files** → `find … -print0 | xargs -0 cmd` (handles spaces and newlines):
   ```bash
   find src tests -name '*.cpp' -print0 | xargs -0 clang-format --dry-run -Werror
   ```
3. **List of options** → an array, passed as an array:
   ```zsh
   args=(--preset test --output-on-failure)
   ctest "${args[@]}"
   ```
   or written literally when short.
4. **Environment variables** → inline assignment in front of each command (`CC=clang CXX=clang++ cmake …`), never `env $VARS cmd`.
5. If explicit splitting is really needed: `${(f)var}` (on newlines) or `${=var}` (on IFS).
6. **Check the real effect** of a bulk operation (rename, substitution) with a counted check before concluding: a pass that edits nothing produces no error.

## Example

- ❌ **Before (wrong)**: `files=$(git ls-files '*.cpp'); clang-format -i $files` → a single argument, `No such file or directory`, nothing is formatted.
- ✅ **After (right)**: `git ls-files -z '*.cpp' '*.hpp' | xargs -0 clang-format -i`, then `git diff --stat` to see the effect.
