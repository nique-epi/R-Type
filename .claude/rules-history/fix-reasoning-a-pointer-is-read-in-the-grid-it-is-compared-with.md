---
description: A pointer position is converted to the grid of what it is compared with, as the screens next to it do, and its test uses a window that scales
trigger: always_on
---

# RULE: A pointer position is read in the grid of what it is compared with — and its test uses a window that scales

## Context

The server map (`ServerMapScreen`) gained hover and click on the stars: moving the pointer over a star chooses its server, a click opens its games. The stars are placed on the interface grid, 480 by 270 pixels.

## Mistake

The screen converted the pointer with `sf::Vector2i(playfieldPointAt_(position))`, a point of the playfield, whose pixels are `BACKGROUND_PIXEL_SIZE` times smaller than those of the interface. The two tests sent the star's own interface pixel as the window position, through `samePlayfieldPoint`, so the comparison matched and both passed. In the game, every hover and click landed several times farther than the star, and the user saw nothing.

## Root cause

The screens next to it (`KeyChip`, `EndScreen`, `LeaderboardScreen`) all read `interfacePixelAt(playfieldPointAt(position))`; the precedent was not searched. And the test fixture used a window where the window pixel and the grid pixel are the same number, the one case where the conversion changes nothing.

## Rule

1. **Before comparing a pointer position with something drawn, name the grid that thing is placed on**, and convert the pointer to it the way the screens next to it do: a widget of the interface canvas reads `interfacePixelAt(playfieldPointAt(position))` (`KeyChip`, `EndScreen`, `LeaderboardScreen`). A cast such as `sf::Vector2i(playfieldPointAt(position))` keeps the playfield scale.
2. **Search the precedent first**: `git grep -n 'playfieldPointAt' -- src` lists how each screen reads the pointer.
3. **A test of the pointer sends a window position that differs from the grid position**: with `samePlayfieldPoint`, multiply the interface pixel by `INTERFACE_PIXEL` before building the event. A test whose window pixel equals the grid pixel passes with or without the conversion.

## Example

- ❌ **Before (wrong)**: `map_.chooseAt(sf::Vector2i(playfieldPointAt_(moved->position)))`, tested with `pointerMove(parisOnTheWholeMap())` → green tests, no hover in the game.
- ✅ **After (right)**: `map_.chooseAt(interfacePixelAt(playfieldPointAt_(moved->position)))`, and the fixture sends `pointerMove(pixel * INTERFACE_PIXEL)`.
