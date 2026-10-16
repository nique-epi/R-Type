---
description: In zsh, path (lowercase) is the array alias of PATH — never name a variable path, fpath, cdpath…
trigger: always_on
---

# RULE: In zsh, `path` (lowercase) is the array alias of `PATH` — assigning it destroys the PATH for the whole call

## Rule

1. **Never name a shell variable `path`, `cdpath`, `fpath`, `manpath`, `module_path`, `watch`, `psvar`** in zsh. Use `file`, `dir`, `target`, `source_file`…
2. **When several basic commands suddenly become "command not found"**, do not diagnose the machine: first look for an assignment of `path` (or an `export PATH=` without `$PATH`) in the preceding call.

### Read-only special parameters stop the line

1. **Never name a loop or `read` variable after a zsh special parameter**: `read -r status file` fails with `read-only variable: status`, `read -r options rest` with `attempt to set slice of associative array`, and either failure stops the whole command line, so nothing written after it runs. `pipestatus` and `argv` are accepted but overwrite what the shell keeps there. Use `state`, `code`, `entry`.
2. **A check whose output stops early is no result**: when a chain prints `read-only variable`, every line it did not print is missing, never "nothing to report"; fix the name and rerun.

## Example

- ❌ **Before (wrong)**: `for path in $(git ls-files '*.hpp'); do clang-format -i "$path"; done` → `command not found: clang-format` from the first iteration.
- ✅ **After (right)**: `git ls-files -z '*.hpp' | xargs -0 clang-format -i`, or a loop over `file`.
