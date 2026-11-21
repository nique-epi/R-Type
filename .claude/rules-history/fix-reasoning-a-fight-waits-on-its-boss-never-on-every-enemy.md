

## Context

The levels of the Pit and of the Ashlands pause the scroll for each boss and resume it when the fight is over; each stage also waited for an empty sky before the next began.

## Mistake

Both waits read `level.enemies()`, every body of side enemy. The user saw the level stand still after a boss died, until the mobs left were killed or flew off screen; the bosses spit mobs (Dunecrawlers, brood mites), so this happened at every fight.

## Root cause

The end of a fight was written as "no enemy left", the end of the waves, while what ends a fight is the death of its boss; the level API had no way to tell a boss from a mob.

## Rule


1. **A stage that waits for a boss reads `level.bosses()`**, the bodies that hold a `Boss`; `level.enemies()` counts the mobs too, which the players need not kill.
2. **A stage never waits for the sky to empty before the next stage**: the mobs left fly on and leave with the scroll.
3. **Before writing any wait of a stage, write what it waits on and who can end it**: a body the players must kill, or one that leaves by itself. A wait that a slow mob can hold is the defect.
4. **Check it with bots**: a fight against a boss that leaves mobs behind reaches the next checkpoint once the boss dies.

## Example

- ❌ **Before (wrong)**: `fight()` paused the scroll, spawned the Sandmaw, then `while level.enemies() > 0 do level.wait(...) end` → after its death the Dunecrawlers it spat kept the scroll paused.
- ✅ **After (right)**: `while level.bosses() > 0 do level.wait(...) end`; bots reach the checkpoint of the core 80 s after the fight starts.
