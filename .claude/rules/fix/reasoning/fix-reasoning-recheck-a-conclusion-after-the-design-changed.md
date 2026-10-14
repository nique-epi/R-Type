---
description: "A conclusion drawn on a design that was replaced is checked again against the code as built before it is repeated"
trigger: always_on
---

# RULE: A conclusion drawn on a replaced design is checked again against the code as built

## Rule

1. **When the design changes during the work** (a plan rewritten, a class reorganized), list the conclusions already given to the user that rest on the old design: conflicts, constraints, risks, things "to adjust later".
2. **Before repeating one of them**, check it against the code as built: open the file, read who owns what and what is passed to whom. A conclusion that no longer holds is withdrawn, out loud, with what changed.
3. **Never carry a constraint from one version of a plan into an explanation of the next** without naming the version it came from.

## Example

- ❌ **Before (wrong)**: the first plan drew sprites straight into the window; after the switch to a `PixelCanvas`, the explanation of another pull request still says "`SfmlDrawSurface` keeps its target from construction, so it will have to be adjusted for the game screen".
- ✅ **After (right)**: read `GameWindow` as built: the surface draws on the entity canvas, which the same owner holds → nothing to adjust on the target; only `PixelCanvas::show()` being non-const remains.
