# RULE: In zsh, `path` (lowercase) is the array alias of `PATH` — assigning it destroys the PATH for the whole call

This rule came with the rules imported when R-Type started; its first incident belongs to another repository. This file starts with its first update on this project.

## Update (2026-10-16) — a read-only special parameter as a loop variable

### Context

Rules pull request after the connection screen: checking that the commits of a branch built with a temporary index hold every rule file of the working tree.

### Mistake

The check looped with `git status --porcelain … | while read -r status file; do …; done`. In zsh `status` is a read-only special parameter: the loop failed with `read-only variable: status`, and the commands written after it in the same call, the commit count and the message checks, never ran.

### Root cause

The rule listed the names that alias a path variable (`path`, `fpath`…), not the other special parameters; `status` is a natural name for the first column of `git status --porcelain`. Measured with zsh 5.9 on macOS: `status` and `options` fail as `read` variables, `pipestatus` and `argv` are accepted but overwrite the shell's values.

### Rule

See the core file, subsection "Read-only special parameters stop the line".

### Example

- ❌ `git status --porcelain | while read -r status file; do …; done; git log …` → `read-only variable: status`, the `git log` never runs.
- ✅ `git status --porcelain | while read -r state file; do …; done; git log …`.
