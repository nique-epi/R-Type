---
description: In zsh, path (lowercase) is the array alias of PATH — never name a variable path, fpath, cdpath…
trigger: always_on
---

# RULE: In zsh, `path` (lowercase) is the array alias of `PATH` — assigning it destroys the PATH for the whole call

## Rule

1. **Never name a shell variable `path`, `cdpath`, `fpath`, `manpath`, `module_path`, `watch`, `psvar`** in zsh. Use `file`, `dir`, `target`, `source_file`…
2. **When several basic commands suddenly become "command not found"**, do not diagnose the machine: first look for an assignment of `path` (or an `export PATH=` without `$PATH`) in the preceding call.

## Example

- ❌ **Before (wrong)**: `for path in $(git ls-files '*.hpp'); do clang-format -i "$path"; done` → `command not found: clang-format` from the first iteration.
- ✅ **After (right)**: `git ls-files -z '*.hpp' | xargs -0 clang-format -i`, or a loop over `file`.
