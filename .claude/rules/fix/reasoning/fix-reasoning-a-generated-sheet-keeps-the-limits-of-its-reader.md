---
description: A sheet written by a script keeps the limits of the reader of the client, read in EntityAppearanceConstants.hpp, and the test that reads the resource runs before anyone plays it
trigger: always_on
---

# RULE: A sheet written by a script keeps the limits of the reader that loads it

Why: the sheets of the Necromancer were rebuilt with every frame of the source; `bone_arm` held 22 frames where the client reads 16 at most, so the client refused the whole content pack and left the server, and the row of the rings was 4246 pixels wide where a sheet is 4096 at most.

## Rule

1. **Before a script writes the appearance of a sheet, read the ranges of the reader**: `src/client/Rendering/Sprites/EntityAppearanceConstants.hpp` (`MOST_FRAMES`, `LARGEST_SHEET_PIXELS`, `FARTHEST_ANCHOR`, `WIDEST_FRAME_SPACING`), and make the script hold them: a table that would pass one is resampled to the most that fit, never written as it is.
2. **When the reader's limit is the problem** (the animation is choppy because frames were dropped), raising it is a change of the sheet format: the constant, its test and `entity-sheets.md` change together.
3. **Run the test that reads every sheet of the resource before anyone plays it**: `EntityAppearances.ReadsEverySheetOfTheFosseResource` for `fosse`. The server reads `[gameplay]` only, so a server that starts proves nothing about `[appearance]`.

## Example

- ❌ **Before (wrong)**: `MAXIMUM_FRAMES = 1000` in the builder → `bone_arm.toml:61:10: appearance.attack_0.strike.frames must be an integer from 1 to 16`, the pack refused by every client.
- ✅ **After (right)**: `MOST_FRAMES` raised to 24 with its test and its page, the builder resamples the rings to the 23 frames that fit in 4096 pixels, and the fosse test passes before the game is played.
