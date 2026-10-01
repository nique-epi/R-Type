---
description: When a change is announced as "neutral here, beneficial there", name the axis that differs, otherwise both claims contradict each other
trigger: always_on
---

# RULE: When I announce a change as "neutral here, beneficial there", NAME the axis that differs

## Rule

1. **Any claim of the form "it changes nothing in X but everything in Y" explicitly names the axis along which X and Y differ**, in the same sentence.
2. **When two true statements read like a contradiction, that is a flaw in my argument**, not in the reader's understanding. Do not repeat the same thing louder: identify the hidden variable and bring it to the front.
3. **Reread looking for the apparent contradiction** before sending a justification that combines "neutral" and "big gain".
4. Also applies to subagent briefs and PR descriptions.

## Example

- ❌ **Before (wrong)**: "Switching the registry to `std::vector` changes nothing performance-wise, the entity count is small. And it cuts the collision system's time by three."
- ✅ **After (right)**: "The **number of accesses** does not change: one per entity per tick, before and after. What changes is **memory locality**: with `std::vector`, the collision system walks contiguous components instead of following pointers, hence the gain."
