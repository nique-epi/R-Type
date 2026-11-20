---
description: A piece of decor is laid where the scroll has brought it and touching the floor or the ceiling it belongs to
trigger: always_on
---

# RULE: A piece of decor is laid where the scroll has brought it, against the floor or the ceiling it belongs to

Why: a stage that began late laid all the decor it had passed at the right edge at once, on top of each other, and a beacon and a lamp of the ceiling were laid in the middle of the corridor.

## Rule

1. **A level lays a piece of decor at the column its scroll gives it**: `DECOR_COLUMN - (level.scroll() - decor.at)`, never at the right edge whatever the scroll. A stage can begin late (a wait for a clear sky), and `level.wait_scroll` of a scroll already passed returns at once.
2. **Every piece rests on what holds it**: compute, from its frame, the row its center needs so that its base touches the floor (row 896) or its top touches the ceiling (row 192), and write it with the piece. A piece drawn for a wall or a floor that hangs from a ceiling is flipped in its sheet so its base touches the ceiling.
3. **A biome without a ceiling holds no hanging piece.**
4. **Look at each piece in the game**, against the map.

## Example

- ❌ **Before (wrong)**: `behaviours.spawn(decor.sheet, { x = DECOR_COLUMN, ... })` and `alarm_beacon = 260`, upright → the core began at 11580, its rods, leaks and vents appeared together at x 2040, and the beacon floated under the ceiling.
- ✅ **After (right)**: `local column = DECOR_COLUMN - math.max(0, level.scroll() - decor.at)`, and the beacon flipped, its base at row 192 (`alarm_beacon = 216`).
