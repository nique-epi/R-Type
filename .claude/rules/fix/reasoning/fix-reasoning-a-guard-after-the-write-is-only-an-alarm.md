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

## Example

- ❌ **Before (wrong)**: copying the payload into the entity's component, then checking that the entity id exists — another entity's component has already been overwritten.
- ✅ **After (right)**: check that the entity exists and the payload size, then write the component.
