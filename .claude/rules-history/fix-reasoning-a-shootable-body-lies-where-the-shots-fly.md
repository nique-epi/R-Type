

## Context

The decor of the Highway Zero stage of the Ashlands lays toxic barrels on the floor, which the ground of the biome places between rows 880 and 928. The band of the biome stops the ships at row 848.

## Mistake

The user could not shoot the barrels: they were too low. The barrel's box was the size of its sprite, 72 units tall, so its top lay between 808 and 856, while the lowest ship (center 816) fires shots spanning 804 to 828.

## Root cause

The box was taken from the sprite, and the reach of the shots was never computed against where the floor puts the body.

## Rule


1. **Before giving a shootable body a row near the floor or the ceiling, compute the rows its box spans and the rows a shot spans from the lowest (highest) ship**: the band bottom less half `SHIP_HEIGHT` (64), plus or minus half the height of the missile box (24). The two must overlap wherever the body may stand, with the ground at its lowest (highest).
2. **When they do not, grow the box towards the ships**, never shrink the reach of the shots; say in the sheet which rows it was grown to reach.
3. **Check it in a game**: a bot flying along the bottom of the band destroys the body, or a probe writes the rows of the box and of the shots.

## Example

- ❌ **Before (wrong)**: `toxic_barrel` 56 × 72, its center 36 above the floor (rows 880 to 928) → its top between 808 and 856, while the shots of the lowest ship span 804 to 828.
- ✅ **After (right)**: its box 136 tall, 32 above its center, its top between 792 and 840 above the floor: every barrel reaches the shots.
