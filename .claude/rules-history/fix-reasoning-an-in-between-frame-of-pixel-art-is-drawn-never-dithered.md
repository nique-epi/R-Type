---
description: An in-between frame of a pixel-art sheet is a drawn pose, never a checkerboard mix of its two neighbours
trigger: always_on
---

# RULE: An in-between frame of pixel art is drawn, never dithered from its neighbours

Why: the Necromancer, the Gloomwyrm vertebrae and their attacks played one frame of two made by a script that mixed the frames around it pixel by pixel in a checkerboard; at the scale of the game each such frame flickered as a speckled ghost of two poses.

## Context

On the branch of the Pit, the sheets of the Necromancer and of the Gloomwyrm segment were made smoother by a script that inserted a frame between each pair of drawn frames.

## Mistake

The inserted frame was a checkerboard of its two neighbours. In game, one frame in two looked speckled; the user saw the boss "not perfect".

## Root cause

A blend that works for photographs was applied to pixel art, where each pixel is a deliberate choice; nothing measured the frames the script wrote.

## Rule

1. **Never make a frame by mixing two frames of a sheet** (a checkerboard, an alpha blend, every other pixel of each): pixel art has no half pixel, and the mix is seen as noise, one frame in two.
2. **To smooth a motion, lengthen the frames the artist drew** (`frame_ms`) or draw the pose in between; a sheet with fewer, cleaner frames is the default.
3. **Before committing a sheet a script touched, measure the share of pixels of each frame that match neither neighbour on a checkerboard** (a frame where about half its changed pixels alternate between the two poses is a mix), and look at the strip scaled up, the frames side by side.
4. **When frames are removed, the timings that count them change with them**: the beats a script waits on an animation (`BEATS` of `necromancer.luau`) equal its frames times `frame_ms`, and the anchors are measured again against the idles (`fix-reasoning-an-anchor-is-measured-against-every-idle-it-leaves.md`).

## Example

- ❌ **Before (wrong)**: `tween_sheet.py` doubled the idle of the Necromancer from 12 to 24 frames, every odd frame a checkerboard of the two around it → the boss shimmered in game.
- ✅ **After (right)**: the odd frames removed, 12 frames at 83 ms, the beats of the rings, fist and gaze recomputed from the new counts, and no frame left whose pixels alternate between its neighbours.
