

## Context

The Pit and the Ashlands lay decor (barrels, billboards, geysers, cables, beacons, lamps) and walkers (Sporecap, Stalagmaw, Dunecrawler, Fixbot, Rail turret) from their Luau scripts, before their LDtk projects place them. The client draws the terrain from the maps of each biome, whose floor and ceiling rise and fall along the level.

## Mistake

Each script wrote one floor row and one ceiling row for the whole level (`FLOOR_ROW = 848`, a ceiling assumed at 192, a floor at 896), and the rows of the decor were computed from them. The user saw fixed mobs and decor in the air, and asked whether it could be fixed. Measured from the maps afterwards: the floor of the highway lies between 880 and 928 units, the ceiling of the shelter between 120 and 160, of the silo between 88 and 152.

## Root cause

The server knew nothing of the terrain: the maps were only read by the client, and the scripts were tuned on one screenshot. A row picked where one piece looked right was taken as the row of every piece.

## Rule


1. **Before giving a row to a body that stands on the floor, hangs from the ceiling or walks along either**, read the ground the client draws under its column: `level.ground(x)` and `level.ceiling(x)` in a level or behaviour script, from the `Floor` and `Ceiling` fields of the LDtk project of the biome.
2. **What the script writes is a gap to that surface** (the center of a barrel 36 units above the floor), never a row of the playfield. A fallback row is only for a biome without ground, and says so.
3. **A body that moves keeps to the ground every tick**, as `along()` does in the mob scripts, and enters at the row the ground gives where it enters.
4. **A new biome project carries its ground**: `export_ldtk.py` writes it; a project exported before gets it from `tools/add_ground.py`.
5. **Prove it in a game**: a probe in a copy of the script writes the ground it read and the row it gave (`error(string.format(…))`), once, and the value read differs from the hand-written row.

## Example

- ❌ **Before (wrong)**: `toxic_barrel = 860` and `alarm_beacon = 216`, rows of the playfield, while the floor of the highway lies between 880 and 928 and the ceiling of the shelter between 120 and 160 → barrels in the air, beacons under the rock.
- ✅ **After (right)**: `toxic_barrel = 36` above `level.ground(column)`, `alarm_beacon = 24` below `level.ceiling(column)`; a probe reads a floor of 888 where the Dunecrawler enters, and it walks at 840.
