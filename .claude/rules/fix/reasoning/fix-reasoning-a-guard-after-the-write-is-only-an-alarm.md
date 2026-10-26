---
description: A guard runs before the write it protects; a check placed after it is only an alarm
trigger: always_on
---

# RULE: A guard runs BEFORE the write it protects — a check placed after it is only an alarm

## Rule

1. **For each guard, name the write it protects, then check that it runs BEFORE it**, in the real execution order. If the write is already done when the check fails, it is an alarm, and it is called that.
2. **When the write cannot be preceded by the check**, first write to a location nobody consumes (temporary buffer, temporary file, separate CI step), check, then publish.
3. **Code reused as is does not exempt you from rereading the order.** A defect found in copied code is reported.
4. **In a PR, the word "guard" is reserved for a check that blocks the write.**

### A check typed in the same command as the write gates it

1. **A check written in the same shell call as the write it protects is the condition of that write**: `if <check>; then <write>; else echo 'BLOCKED'; fi`. A check followed by `;` and then the write only prints: the write runs whatever the check found.
2. **A check whose output needs reading** (a `grep` for forbidden words that also matches allowed ones, a diff to look at) **runs alone, in the call before the write**; the write goes in the next call, once that output was read.

## Example

- ❌ **Before (wrong)**: copying the payload into the entity's component, then checking that the entity id exists — another entity's component has already been overwritten.
- ✅ **After (right)**: check that the entity exists and the payload size, then write the component.
- ❌ **Before (wrong)**: `grep -n -i -E 'claude|anthropic|generated with|co-authored' pr-body.md; gh pr create --draft … --body-file pr-body.md` → the pull request was created before the five matches were read; all were `.claude/` paths, so nothing forbidden was published, by luck.
- ✅ **After (right)**: the `grep` runs alone and its matches are read; `gh pr create` goes in the next call.
