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

## Update (2026-10-17) — a path derived inside a loop, named `path`

### Context

Cutting the options menu into commits: intermediate versions of eight C++ files were written under `scratchpad/commits/<commit>/<repository path>`, and a loop checked that each was formatted, deriving the repository path from the file name to pass it to `clang-format --assume-filename`.

### Mistake

The derived path was assigned to `path` (`path=${file#*/}`). From the first iteration `clang-format`, `diff`, `git` and `wc` were "command not found", and every file was printed as "NEEDS FORMAT". Nothing was written, since the loop only read files, but its whole output was void.

### Root cause

The rule names `path` among the forbidden names, and it was still typed: the variable stood for "the path of the file", so the forbidden word was the natural one, and no check was made on the command text before sending it.

### Rule (added)

1. **A name that says "the path of this file" is the trap**: when a loop derives one path from another (`relative=${file#*/}`, the path inside a commit folder, a path without its prefix), name it `relative`, `target`, `source_file` or `inside`, never `path`. The assignment runs at the first iteration and every later command of the loop fails.
2. **Before sending a command that holds a loop or an assignment, read the name on the left of each `=` and of each `for … in`** against the list of point 1 of the rule. This check is made on the command text, not remembered.

## Update (2026-10-18) — `path` again, in a staging loop

### Context
Splitting the client animations into commits, intermediate file contents were staged with `git hash-object` and `git update-index --cacheinfo` in a loop over their paths.

### Mistake
The loop variable was named `path`. Its first assignment replaced `PATH`, so every `git` and `tail` of the call failed with `command not found`. Only the `git rm --cached` placed before the loop had run, which left three deletions alone in the index.

### Root cause
The loop staged paths rather than reading files, and the name was chosen from what the loop held. The check on the left of each `for … in` that the rule asks for was not made.
