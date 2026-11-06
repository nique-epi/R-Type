---
description: A pointer position is converted to the grid of what it is compared with, as the screens next to it do, and its test uses a window that scales
trigger: always_on
---

# RULE: A pointer position is read in the grid of what it is compared with — and its test uses a window that scales

Why: the server map compared `playfieldPointAt_()`, a point of the playfield, with stars placed on the interface grid; every hover and click missed in the game, and the tests passed because their window did not scale.

## Rule

1. **Before comparing a pointer position with something drawn, name the grid that thing is placed on**, and convert the pointer to it the way the screens next to it do: a widget of the interface canvas reads `interfacePixelAt(playfieldPointAt(position))` (`KeyChip`, `EndScreen`, `LeaderboardScreen`). A cast such as `sf::Vector2i(playfieldPointAt(position))` keeps the playfield scale.
2. **Search the precedent first**: `git grep -n 'playfieldPointAt' -- src` lists how each screen reads the pointer.
3. **A test of the pointer sends a window position that differs from the grid position**: with `samePlayfieldPoint`, multiply the interface pixel by `INTERFACE_PIXEL` before building the event. A test whose window pixel equals the grid pixel passes with or without the conversion.

## Example

- ❌ **Before (wrong)**: `map_.chooseAt(sf::Vector2i(playfieldPointAt_(moved->position)))`, tested with `pointerMove(parisOnTheWholeMap())` → green tests, no hover in the game.
- ✅ **After (right)**: `map_.chooseAt(interfacePixelAt(playfieldPointAt_(moved->position)))`, and the fixture sends `pointerMove(pixel * INTERFACE_PIXEL)`.
