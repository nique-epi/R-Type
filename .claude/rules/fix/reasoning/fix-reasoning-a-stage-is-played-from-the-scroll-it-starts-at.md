---
description: The waves and the decor of a stage are counted from the scroll its stage really starts at, never from the scroll it was written for
trigger: always_on
---

# RULE: A stage is played from the scroll it starts at — a wait before it moves everything it lays

Why: the shelter of the Ashlands laid its decor at the scroll values written for a stage starting at 5280; the stage before waited for a clear sky with the scroll running, so the shelter began later and laid every piece already due at once, left of where its biome came in, over the biome that left.

## Rule

1. **Before writing the waves or the decor of a stage at scroll values, list what can delay its start**: a wait for a clear sky with the scroll running, a fight, a restart from a checkpoint. Each one moves the scroll the stage starts at, and the biome it names with it.
2. **Play the stage from where it starts**: add to every `at` and to its end the lateness of its start (`lateness(planned)` in `ashlands.luau`), so its pieces keep their distance from the entrance of its biome.
3. **Walk a late start through `place()`**: a piece is laid left of the entrance of its biome when its `at` plus the gap past the right edge is below the scroll its biome began at; that piece shows over the biome that leaves, with no ground under it.

## Example

- ❌ **Before (wrong)**: `play(SHELTER, SHELTER_DECOR)` with `at = 6120` for a live cable, after a stage that waited for a clear sky past 6120 → the cable laid at once, over the highway that leaves, hanging from nothing.
- ✅ **After (right)**: `local shift = lateness(END_OF_THE_HIGHWAY)`, then `play(SHELTER, SHELTER_DECOR, shift)` and `level.wait_scroll(END_OF_THE_SHELTER + shift)`.
