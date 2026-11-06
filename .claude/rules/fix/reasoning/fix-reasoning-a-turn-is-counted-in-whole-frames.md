---
description: A motion stepped at a frame rate counts whole frames in integers, never a remainder of a rounded frame duration
trigger: always_on
---

# RULE: A motion stepped at a frame rate is counted in whole frames, never by a remainder of a rounded frame duration

Why: one frame at 24 a second is 41 666 666.67 ns, so `elapsed - elapsed % frame` drifts by 480 ns over 30 s and a whole turn of the planet stopped short of where it started.

## Rule

1. **Before stepping a value at a frame rate** (an animation, a turn, a blink), check whether one frame is a whole number of the clock's ticks: `1 s / 24` in nanoseconds is not.
2. **When it is not, count the frames in integers**: `frame = elapsed.count() * FRAMES_PER_SECOND / ticksPerSecond`, then read the angle as `frame % framesPerTurn` over `framesPerTurn`. Never `elapsed - (elapsed % frame)` with a `frame` already rounded.
3. **Test the end of a whole period**: after exactly one turn the value is back where it started, `EXPECT_EQ`, not `EXPECT_NEAR`.

## Example

- ❌ **Before (wrong)**: `frame = 1 s / 24` (41 666 666 ns), `shown = elapsed - elapsed % frame`, spin read on `shown` → at 30 s the spin is 6.2831852, not 0.
- ✅ **After (right)**: `framesIn(30 s) = 720`, `720 % 720 = 0` → spin 0 at 30 s, π at 15 s exactly.
