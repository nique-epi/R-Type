---
description: When a measurement is available, quantify each hypothesis's prediction and eliminate those it refutes before coding
trigger: always_on
---

# RULE: Use the measurement to falsify hypotheses before coding

## Rule

When the user provides a measurement (screenshot, number, timing, timestamped log, profile):

1. **Quantify the prediction of each hypothesis** before writing a line. Hypothesis A predicts 16 ms per frame, hypothesis B predicts 33 ms, the measurement says 33 ms → A is dead, without compiling anything.
2. **Check that the retained hypothesis also explains the current code**: if it does not explain why the bug already exists, a factor is missing.
3. When several hypotheses survive and you cannot decide, **ship the fix that is right under ALL surviving hypotheses**, never the one right only under the favorite.
4. **"I couldn't verify" is not a disclaimer that allows guessing**: it is the signal to look for the missing data, or to apply point 3.

## Example

- ❌ **Before (wrong)**: "the game runs at 30 FPS, rendering is slow" → optimize rendering → no change.
- ✅ **After (right)**: slow rendering would predict a varying frame time; the measurement says a stable 33.3 ms → it is a cap, not slowness → find the `setFramerateLimit(30)` or the vsync.
