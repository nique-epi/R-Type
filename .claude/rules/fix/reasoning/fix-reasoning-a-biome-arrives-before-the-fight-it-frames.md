---
description: A biome named for a boss fight is named early enough that its entrance has crossed the screen when the scroll pauses
trigger: always_on
---

# RULE: A biome a boss is fought in is named early enough that its entrance has crossed the screen before the scroll pauses

Why: the core was named at the start of the stage of the Glowheart, after a wait for a clear sky of unknown length; the scroll paused for the fight before the entrance of the core had crossed the screen, and the boss was fought between two biomes.

## Rule

1. **Name the biome of a fight at a scroll the script fixes**, never after a wait of unknown length (`wait_for_a_clear_sky`): the end of the stage before, right after its `level.wait_scroll`.
2. **Check the distance**: between the scroll the biome is named at and the scroll the fight pauses at, at least the width of the playfield, 1920 units, for the near layer, whose entrance moves with the scroll.
3. **Look at the arrival of the boss in the game** with the background behind it.

### A biome is named as the stage before ends, never on the first line of its stage

1. **A checkpoint keeps the biome of the moment its stage starts**: a `level.biome` on the first line of a stage is not in it. After a restart the client shows the previous biome, then that line brings the entrance of the new one back from the right edge with nothing behind it.
2. **Name the biome on the last line of the stage before** (after its fight, once the scroll moves again); a game started at the stage with `--stage` gets it from the `starts` option of `level.register`.
3. **Search every level of the repository for it**, not only the one being written: `git grep -n -A1 'function()' -- 'resources/*.luau' | grep 'level.biome'` must print nothing.

## Example

- ❌ **Before (wrong)**: stage 3 begins with `level.biome("cendres_coeur")` after the shelter waited for a clear sky, then pauses at 12960 → the Glowheart in front of half the shelter.
- ✅ **After (right)**: the shelter names `cendres_coeur` right after `level.wait_scroll(10800)`, 2160 units before the pause.
