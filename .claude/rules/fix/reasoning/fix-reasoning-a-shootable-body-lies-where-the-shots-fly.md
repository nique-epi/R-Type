---
description: A body the players must shoot overlaps the rows their shots reach, from the lowest and highest rows a ship can fly at
trigger: always_on
---

# RULE: A body the players must shoot overlaps the rows their shots fly at

Why: the toxic barrels of the Ashlands stood on the floor with a box 72 units tall; a ship flies no lower than its band lets it, and its shots passed above most barrels.

## Rule

1. **Before giving a shootable body a row near the floor or the ceiling, compute the rows its box spans and the rows a shot spans from the lowest (highest) ship**: the band bottom less half `SHIP_HEIGHT` (64), plus or minus half the height of the missile box (24). The two must overlap wherever the body may stand, with the ground at its lowest (highest).
2. **When they do not, grow the box towards the ships**, never shrink the reach of the shots; say in the sheet which rows it was grown to reach.
3. **Check it in a game**: a bot flying along the bottom of the band destroys the body, or a probe writes the rows of the box and of the shots.

## Example

- ❌ **Before (wrong)**: `toxic_barrel` 56 × 72, its center 36 above the floor (rows 880 to 928) → its top between 808 and 856, while the shots of the lowest ship span 804 to 828.
- ✅ **After (right)**: its box 136 tall, 32 above its center, its top between 792 and 840 above the floor: every barrel reaches the shots.
