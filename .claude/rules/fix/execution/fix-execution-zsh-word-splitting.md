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

### `for name in $list` runs once, and `grep` then reads every line as a pattern

1. **A loop over the words or lines of a variable is never `for name in $list`**: zsh runs it once, with the whole text in `name`. Handed to `grep -w "$name"`, that text is a list of patterns, one per line, so the match succeeds as soon as any one of them is in the file, and the loop prints the whole list.
2. **Loop with `${(f)list}` (one item per line) or `${=list}` (one per word)**, or do the matching in a short Python script that splits explicitly.
3. **A filter whose output repeats its whole input for every file is this bug**: rerun it before reading any result.

## Example

- ❌ **Before (wrong)**: `files=$(git ls-files '*.cpp'); clang-format -i $files` → a single argument, `No such file or directory`, nothing is formatted.
- ✅ **After (right)**: `git ls-files -z '*.cpp' '*.hpp' | xargs -0 clang-format -i`, then `git diff --stat` to see the effect.
