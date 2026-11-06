---
description: A motion stepped at a frame rate counts whole frames in integers, never a remainder of a rounded frame duration
trigger: always_on
---

# RULE: A motion stepped at a frame rate is counted in whole frames, never by a remainder of a rounded frame duration

## Context

The ringed planet of the server map turns once in 30 s and its ring in 20 s, both shown at 24 frames a second as the mockup draws them. `turnedPlanet()` took the time elapsed since the map opened and gave the spin and the drift of the frame shown.

## Mistake

The first version cut the elapsed time into frames with `frame = 1 s / 24` in nanoseconds and `shown = elapsed - (elapsed % frame)`, then read the turn on `shown`. Two tests turned red before the commit: after 30 s the spin was 6.2831852 instead of 0, and after 15 s it was 3.1415926033 instead of π.

## Root cause

1 s / 24 is 41 666 666.67 ns; the integer division kept 41 666 666 ns. Over 720 frames the lost two thirds of a nanosecond add up to 480 ns, so the last frame of a turn started 480 ns before the end of the turn, and the angle read on it fell short.

## Rule

1. **Before stepping a value at a frame rate** (an animation, a turn, a blink), check whether one frame is a whole number of the clock's ticks: `1 s / 24` in nanoseconds is not.
2. **When it is not, count the frames in integers**: `frame = elapsed.count() * FRAMES_PER_SECOND / ticksPerSecond`, then read the angle as `frame % framesPerTurn` over `framesPerTurn`. Never `elapsed - (elapsed % frame)` with a `frame` already rounded.
3. **Test the end of a whole period**: after exactly one turn the value is back where it started, `EXPECT_EQ`, not `EXPECT_NEAR`.

## Example

- ❌ **Before (wrong)**: `frame = 1 s / 24` (41 666 666 ns), `shown = elapsed - elapsed % frame`, spin read on `shown` → at 30 s the spin is 6.2831852, not 0.
- ✅ **After (right)**: `framesIn(30 s) = 720`, `720 % 720 = 0` → spin 0 at 30 s, π at 15 s exactly.
