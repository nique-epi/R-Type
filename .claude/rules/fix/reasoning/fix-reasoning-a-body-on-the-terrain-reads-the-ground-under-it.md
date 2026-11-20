---
description: A body that stands on the terrain, hangs from it or walks on it takes its row from the ground of the biome under its column, never from one row for the whole level
trigger: always_on
---

# RULE: A body on the terrain reads the ground under its column — one row for a whole level floats or sinks

Why: the decor and the walkers of the Pit and of the Ashlands stood on one floor row and hung from one ceiling row written in their scripts; the maps rise and fall by tens of units, so they floated above the rock or sank into it.

## Rule

1. **Before giving a row to a body that stands on the floor, hangs from the ceiling or walks along either**, read the ground the client draws under its column: `level.ground(x)` and `level.ceiling(x)` in a level or behaviour script, from the `Floor` and `Ceiling` fields of the LDtk project of the biome.
2. **What the script writes is a gap to that surface** (the center of a barrel 36 units above the floor), never a row of the playfield. A fallback row is only for a biome without ground, and says so.
3. **A body that moves keeps to the ground every tick**, as `along()` does in the mob scripts, and enters at the row the ground gives where it enters.
4. **A new biome project carries its ground**: `export_ldtk.py` writes it; a project exported before gets it from `tools/add_ground.py`.
5. **Prove it in a game**: a probe in a copy of the script writes the ground it read and the row it gave (`error(string.format(…))`), once, and the value read differs from the hand-written row.

## Example

- ❌ **Before (wrong)**: `toxic_barrel = 860` and `alarm_beacon = 216`, rows of the playfield, while the floor of the highway lies between 880 and 928 and the ceiling of the shelter between 120 and 160 → barrels in the air, beacons under the rock.
- ✅ **After (right)**: `toxic_barrel = 36` above `level.ground(column)`, `alarm_beacon = 24` below `level.ceiling(column)`; a probe reads a floor of 888 where the Dunecrawler enters, and it walks at 840.
