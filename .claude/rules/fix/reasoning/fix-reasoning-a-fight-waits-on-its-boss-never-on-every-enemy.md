---
description: A stage that waits for a boss waits on the boss alone (level.bosses()), never on every enemy; a stage never waits for the sky to empty before the next one
trigger: always_on
---

# RULE: A fight waits on its boss, never on every enemy of the world

Why: the Ashlands and the Pit paused the scroll for a boss and resumed it once `level.enemies()` read 0; the mobs the boss left, or that lingered, held the level still after the boss died, and each stage also waited for an empty sky before the next began.

## Rule

1. **A stage that waits for a boss reads `level.bosses()`**, the bodies that hold a `Boss`; `level.enemies()` counts the mobs too, which the players need not kill.
2. **A stage never waits for the sky to empty before the next stage**: the mobs left fly on and leave with the scroll.
3. **Before writing any wait of a stage, write what it waits on and who can end it**: a body the players must kill, or one that leaves by itself. A wait that a slow mob can hold is the defect.
4. **Check it with bots**: a fight against a boss that leaves mobs behind reaches the next checkpoint once the boss dies.

## Example

- ❌ **Before (wrong)**: `fight()` paused the scroll, spawned the Sandmaw, then `while level.enemies() > 0 do level.wait(...) end` → after its death the Dunecrawlers it spat kept the scroll paused.
- ✅ **After (right)**: `while level.bosses() > 0 do level.wait(...) end`; bots reach the checkpoint of the core 80 s after the fight starts.
