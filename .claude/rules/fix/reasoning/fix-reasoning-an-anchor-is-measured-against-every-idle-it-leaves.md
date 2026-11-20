---
description: An attack animation's anchor is measured against every idle it starts from and returns to
trigger: always_on
---

# RULE: An anchor of an attack animation is measured against every idle the body leaves and goes back to

Why: the gaze of the Necromancer declared `anchor_x = 0` where its body matched the idle at -1, and its enraged idle sat one pixel right of the idle; the boss jumped a pixel at the start of four attacks.

## Rule

1. **Before writing or changing `anchor_x`, `anchor_y`, or the width or height of a frame of a sheet with attacks**, measure the anchor: place the frame as the client does, top-left at `nearestPixel(center − size / 2)` plus the anchor (`SfmlDrawSurface::placedSprite`), and find the anchor that best overlaps its opaque pixels with those of the idle frame.
2. **Measure each transition the body plays**: the first frame of each `signal` against `idle` and against `enraged_idle`, and the last frame of each `recover` that goes back to idle against both. A difference of one pixel is the defect.
3. **An odd width or height moves the frame by half a pixel**, which the rounding turns into a whole one: measure with that rounding, never with `w / 2` in real numbers.
4. **`enraged_idle` takes no anchor**: to move it, give its cells the width that centers its body (two transparent columns on the right move it one pixel left), and repack its row of the sprite sheet.

## Example

- ❌ **Before (wrong)**: `[appearance.attack_3.signal]` with `anchor_y = -3` alone, and `enraged_idle` 144 wide → the body jumps one pixel as the gaze starts, and as every attack starts once enraged.
- ✅ **After (right)**: `anchor_x = -1` on the gaze, `anchor_y = -1` on the recover of attack 2, `enraged_idle` 146 wide; every signal and recover then overlaps both idles at the anchor it declares.
