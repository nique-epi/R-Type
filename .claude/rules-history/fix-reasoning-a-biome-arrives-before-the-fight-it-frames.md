---
description: A biome named for a boss fight is named early enough that its entrance has crossed the screen when the scroll pauses
trigger: always_on
---

# RULE: A biome a boss is fought in is named early enough that its entrance has crossed the screen before the scroll pauses

Why: the core was named at the start of the stage of the Glowheart, after a wait for a clear sky of unknown length; the scroll paused for the fight before the entrance of the core had crossed the screen, and the boss was fought between two biomes.

## Context

The Ashlands change biome between stages with `level.biome`, the new background arriving through its entrance as the scroll goes.

## Mistake

The stage of the Glowheart named its biome as it began. The stage before ended with a wait for a clear sky, so the stage began later than 10800, and the fight paused at 12960 with less than a screen of scroll since the biome changed.

## Root cause

The distance between the biome and the pause was assumed from the scrolls written in the script, without counting the wait between them.

## Rule

1. **Name the biome of a fight at a scroll the script fixes**, never after a wait of unknown length (`wait_for_a_clear_sky`): the end of the stage before, right after its `level.wait_scroll`.
2. **Check the distance**: between the scroll the biome is named at and the scroll the fight pauses at, at least the width of the playfield, 1920 units, for the near layer, whose entrance moves with the scroll.
3. **Look at the arrival of the boss in the game** with the background behind it.

## Example

- ❌ **Before (wrong)**: stage 3 begins with `level.biome("cendres_coeur")` after the shelter waited for a clear sky, then pauses at 12960 → the Glowheart in front of half the shelter.
- ✅ **After (right)**: the shelter names `cendres_coeur` right after `level.wait_scroll(10800)`, 2160 units before the pause.
