---
description: In zsh, unquoted ?, *, [, {, ^, # and ~ are expanded, and so is a leading = — a failed expansion cancels the whole line
trigger: always_on
---

# RULE: In zsh, unquoted `?`, `*`, `[` and a leading `=` are EXPANDED — and a failed expansion stops the whole line

## Rule

1. **Any argument containing `?`, `*`, `[`, `]`, `{`, `}`, `^`, `#` or `~` that is not an intended glob goes in single quotes**: URL with a query string, `gh api '…?per_page=100'`, `jq` / `--jq` filters with brackets, regexes, git pathspecs (`git ls-files '*.cpp'`).
2. **Never start an unquoted word with `=`**: for a separator, `echo '-----'`.
3. **`no matches found` or `<word> not found` in zsh means the command did NOT run.** Do not read its empty output as a result: fix the quoting and rerun.

### An intended glob that may match nothing

1. **A glob meant to match "every file of a folder, if any"** (cleaning an output folder before a run, collecting logs) fails in zsh on an empty folder and stops the whole `&&` chain. Use `find <dir> -name '<pattern>' -delete` (or `-print0 | xargs -0`), or the zsh qualifier `(N)`: `rm -f -- "$dir"/*.txt(N)`.
2. **A run whose first command failed has no result**: when a chain prints `no matches found`, every later output line (an empty list, a missing file) is ignored and the chain is rerun.

### The value of an option and a separator are quoted like any other word

1. **A glob in the value of an option is still a glob**: `--include=*.cpp` is expanded, finds no file named so, and the line stops. Write `--include='*.cpp'`.
2. **A separator between two outputs is always a quoted literal**: `echo '-----'`. A bare `echo =====` stops the line too, so every command written after it in the same call never runs.
3. **Before sending a command, read each unquoted word that holds `=`, `*`, `?` or `[`** and quote it unless it is an intended glob.

### A search by file kind uses `git grep` with quoted pathspecs, never `grep --include`

1. **To search a kind of file, write `git grep -n '<pattern>' -- '*.cpp' '*.hpp'`**, not `grep -rn <pattern> --include=…`: the pathspecs are separate quoted words, so no option ever carries a glob. `git grep` searches tracked files only; add `--untracked` for files not added yet.
2. **When `grep -r` is unavoidable** (a folder outside the repository), filter afterwards instead: `grep -rn '<pattern>' <dir> | grep -E '\.(cpp|hpp):'`.

### A separator is made of dashes only

1. **Separate two outputs with `echo '-----'` and nothing else**: dashes are never expanded by zsh, so the separator works even when the quotes are forgotten. A separator of `=` signs is never typed, quoted or not.

## Example

- ❌ **Before (wrong)**: `gh api repos/nique-epi/R-Type/pulls?state=all --jq .[].title; echo =====` → `no matches found`, then `===== not found`: nothing runs.
- ✅ **After (right)**: `gh api 'repos/nique-epi/R-Type/pulls?state=all' --jq '.[].title'; echo '-----'`.
- ❌ **Before (wrong)**: `mkdir -p "$S/tidy" && rm -f "$S/tidy"/*.txt && git diff --name-only … > "$S/tidy/files.txt"` on a new folder → `no matches found`, nothing ran, then "files with nonzero exit:" printed empty.
- ✅ **After (right)**: `find "$S/tidy" -name '*.txt' -delete && …`, and the empty output of the failed run never read as "no failure".
