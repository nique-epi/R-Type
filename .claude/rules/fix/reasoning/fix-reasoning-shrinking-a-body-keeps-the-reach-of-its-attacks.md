---
description: A body scaled down keeps what its attacks reach on the playfield; every place an attack lands is computed on screen before and after
trigger: always_on
---

# RULE: Scaling a body down scales what comes out of it, never what its attacks reach

Why: the Necromancer was drawn at half size and every offset of its script halved with it while it moved to the right; its arms, spikes and gaze then landed between x 900 and 1400, and the left of the Pit, where the ships fly, was out of reach.

## Rule

1. **Before scaling the offsets of a body's script or sheet, sort them in two**: what comes out of its frames (a hand, a fist, a mask, a hitbox) follows the frames; what reaches across the playfield (lanes, fissures, a beam, a wave of spikes) keeps its place on the playfield.
2. **Compute where each attack lands on screen, before and after**, center plus offset, in a throwaway script, and compare with where the ships fly (the band of the biome): an attack that no longer reaches the ships is the defect.
3. **A sprite too short for a reach is tiled**, several bodies end to end, never shrunk with the reach.

## Example

- ❌ **Before (wrong)**: `FISSURE_FIRST` -1194 halved to -597 with the boss moved from x 1354 to 1600 → the arms rise between x 1003 and 1363 instead of 160 to 800.
- ✅ **After (right)**: the fissures at -1440 from x 1600, back at x 160; the beam of 1600 units drawn by three pieces of 472.
