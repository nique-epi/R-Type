# RULE: An anchor of an attack animation is measured against every idle the body leaves and goes back to

## Context

The sheet of the Necromancer (`resources/fosse/entities/necromancer.toml`) draws its idle 142 × 110 and its attacks in frames of other sizes, each phase placed with `anchor_x` and `anchor_y`. The user saw the boss jump between its idle and the start of some attacks.

## Mistake

The anchors were written by the script that packed the sheet, from the bounding boxes of the frames, and never compared with the idle on screen. The signal of the gaze declared `anchor_x = 0` where its body matched the idle at -1; the recover of attack 2 declared `anchor_y = 0` where it matched at -1; the enraged idle, 144 wide, sat one pixel right of the idle, so every attack after the rage jumped by a pixel too.

## Root cause

An anchor was treated as a property of a frame alone, when it only means something relative to the frame the body comes from. The odd widths and heights add a half pixel the client rounds, which a measure in real numbers cannot see.

## Rule

See `.claude/rules/fix/reasoning/fix-reasoning-an-anchor-is-measured-against-every-idle-it-leaves.md`.

## Example

- ❌ **Before (wrong)**: the gaze signal at `anchor_y = -3` alone; `enraged_idle` 144 wide.
- ✅ **After (right)**: `anchor_x = -1` on the gaze, `anchor_y = -1` on the recover of attack 2, `enraged_idle` 146 wide; a measurement against both idles finds every declared anchor best.
