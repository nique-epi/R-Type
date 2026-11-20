---
description: A boss weak point is judged by the share of the fight it is open, not by one hit that went through
trigger: always_on
---

# RULE: A boss weak point is judged by the share of the fight it stays open — one hit that went through proves nothing

Why: the Glowheart opened its core only during the 2 s purge after a Meltdown, every third attack; bots took 1 point off it once and it was reported as working, and the player could not hurt it.

## Rule

1. **Before shipping a boss whose `pre_hit` opens only at some moments, compute the share of its cycle it is open**: the milliseconds open over the milliseconds of a whole rotation of its attacks, written next to the code. Under a third, the window is too narrow for a player who must also dodge.
2. **Open it at the moments a player can aim**: its rest, its signals and its recovers, and close it only while it strikes.
3. **The probe of a fight reports the health lost over a fixed time** (60 s against bots), never the first point taken.

### Every boss of the change, not only the one reported

1. **When one boss of a change is found too hard to hit, compute the window of every other boss of the change the same way**, before handing the fix back: the same mistake sits in each `pre_hit` written with the same idea.
2. **A window that opens on the row a strike then burns is no window**: the player cannot stay on it. Count only the moments the row is safe.

## Example

- ❌ **Before (wrong)**: `purging` true for the 2000 ms purge of a Meltdown, one attack in three → open about 2 s of every 9; a probe read 99 of 100 and called it hittable.
- ✅ **After (right)**: closed only during the Tendrils and the burn of the Meltdown, open the rest of the cycle; the probe reads the health lost over the fight.
