---
description: A shared text (pull request description, issue, comment) is replaced only if it is unchanged since it was read, and the check blocks the write
trigger: always_on
---

# RULE: A shared text is replaced only if it is unchanged since it was read — and the check blocks the write

## Rule

1. **A pull request description, an issue or a comment is shared**: a teammate or a bot (CodeRabbit regenerates its release notes after each push) may edit it between my read and my write. Replacing it whole with a version built from an older read silently undoes their edit.
2. **Read it right before the write, compare with the read the new version was built from, and write only if they are equal**, in the same command: `if diff -q … ; then gh api -X PATCH … ; else echo 'CHANGED: not patching'; fi`. A comparison that only prints a message and lets the write run is not a check.
3. **When it changed, rebuild the new version from the current text**: keep the edits of others, apply mine again on top.
4. **After the write, read the edit history** (`userContentEdits` in the GraphQL API) when anything looks off: it names who edited and when, and keeps the diff of each edit, from which an overwritten version can be restored.

## Example

- ❌ **Before (wrong)**: `gh api … | diff -q - read.md && echo 'unchanged'; gh api -X PATCH … -F body=@new.md` → CodeRabbit had regenerated its notes after the push; the message did not print, the PATCH ran anyway and put its older notes back.
- ✅ **After (right)**: the PATCH runs inside the `if` of the comparison; when the text changed, the new version is rebuilt from the current text.
