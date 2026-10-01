---
description: In zsh, unquoted ?, *, [, {, ^, # and ~ are expanded, and so is a leading = — a failed expansion cancels the whole line
trigger: always_on
---

# RULE: In zsh, unquoted `?`, `*`, `[` and a leading `=` are EXPANDED — and a failed expansion stops the whole line

## Rule

1. **Any argument containing `?`, `*`, `[`, `]`, `{`, `}`, `^`, `#` or `~` that is not an intended glob goes in single quotes**: URL with a query string, `gh api '…?per_page=100'`, `jq` / `--jq` filters with brackets, regexes, git pathspecs (`git ls-files '*.cpp'`).
2. **Never start an unquoted word with `=`**: for a separator, `echo '-----'`.
3. **`no matches found` or `<word> not found` in zsh means the command did NOT run.** Do not read its empty output as a result: fix the quoting and rerun.

## Example

- ❌ **Before (wrong)**: `gh api repos/nique-epi/R-Type/pulls?state=all --jq .[].title; echo =====` → `no matches found`, then `===== not found`: nothing runs.
- ✅ **After (right)**: `gh api 'repos/nique-epi/R-Type/pulls?state=all' --jq '.[].title'; echo '-----'`.
