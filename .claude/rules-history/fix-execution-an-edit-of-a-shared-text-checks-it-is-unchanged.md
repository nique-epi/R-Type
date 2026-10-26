# RULE: A shared text is replaced only if it is unchanged since it was read — and the check blocks the write

## Context

After pushing review fixes on the status band pull request (RTY-151), its description had to be updated with the new changes and test counts. The description also held release notes that CodeRabbit writes at its bottom.

## Mistake

The new description was built from a read taken before the push. Right before the PATCH, the command compared the live text with that read and printed "unchanged" when equal, but the PATCH ran on the next line regardless. CodeRabbit had regenerated its notes after the push (one line added), so the PATCH put its older notes back. The edit history (`userContentEdits`) showed it; the overwritten notes were restored from the diff it keeps.

## Root cause

The comparison was written as an observation, not as a gate: `diff -q … && echo 'unchanged'; gh api -X PATCH …` runs the PATCH whatever the result.

## Rule

Read the shared text right before the write, compare it with the read the new version was built from, and write only inside the branch where they are equal; when it changed, rebuild from the current text. After the write, read the edit history when anything looks off.

## Example

- ❌ **Before (wrong)**: `diff -q … && echo 'unchanged'; gh api -X PATCH …`.
- ✅ **After (right)**: `if diff -q … ; then gh api -X PATCH … ; else echo 'CHANGED: not patching'; fi`.
