---
description: A new highlight flag is cleared by every input that sets the highlight it excludes, not only by the input that sets it
trigger: always_on
---

# RULE: A new highlight is cleared by every path that sets the highlight it excludes

## Rule

1. **Before adding a flag that shows something instead of another** (a bubble, corners, a selection), list every place that sets the other flag: `grep -n '<otherFlag> =' <file>`. The pointer, the arrows, Tab and a click each set it.
2. **At each of those places, decide what the new flag becomes**, or derive what is painted from both flags in one expression (`hangarShown_ && !chosenShown_`). A flag cleared only where it is set stays on when another input moves the highlight.
3. **Test the crossing**: set the new highlight with one input, move the other with another input, assert that only one shows.

## Example

- ❌ **Before (wrong)**: the hangar star of the server map set `hangarShown_` on pointer moves only; an arrow then set `chosenShown_` and both bubbles were painted on top of each other.
- ✅ **After (right)**: the star is painted with `hangarShown_ && !chosenShown_`, and a test points at the star, presses Right, and asserts its hint is gone.
