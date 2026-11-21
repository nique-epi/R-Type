

## Context

The level of the Ashlands plays each stage as a list of waves and decor at scroll values, written for a stage that starts where the previous one ends on paper (`END_OF_THE_HIGHWAY` = 5280 for the shelter). Its biome is named at the end of the stage before, after its fight or its wait for a clear sky.

## Mistake

The user saw a hanging live cable of the shelter appear over the highway that was leaving, during the change of biome. The stage before had waited for a clear sky with the scroll running, so the shelter started later than 5280, and `place()` laid each piece already due further left, at once, left of the entrance of the shelter. Left of the first map of its biome, `level.ceiling` reads nothing, and the cable hung at the fallback row, in the air.

## Root cause

The scroll values of a stage were taken as absolute, while the start of the stage moved with every wait before it. `place()` was written to lay late pieces further left, which hid the lateness instead of removing it.

## Rule


1. **Before writing the waves or the decor of a stage at scroll values, list what can delay its start**: a wait for a clear sky with the scroll running, a fight, a restart from a checkpoint. Each one moves the scroll the stage starts at, and the biome it names with it.
2. **Play the stage from where it starts**: add to every `at` and to its end the lateness of its start (`lateness(planned)` in `ashlands.luau`), so its pieces keep their distance from the entrance of its biome.
3. **Walk a late start through `place()`**: a piece is laid left of the entrance of its biome when its `at` plus the gap past the right edge is below the scroll its biome began at; that piece shows over the biome that leaves, with no ground under it.

## Example

- ❌ **Before (wrong)**: `play(SHELTER, SHELTER_DECOR)` with `at = 6120` for a live cable, after a stage that waited for a clear sky past 6120 → the cable laid at once, over the highway that leaves, hanging from nothing.
- ✅ **After (right)**: `local shift = lateness(END_OF_THE_HIGHWAY)`, then `play(SHELTER, SHELTER_DECOR, shift)` and `level.wait_scroll(END_OF_THE_SHELTER + shift)`.
