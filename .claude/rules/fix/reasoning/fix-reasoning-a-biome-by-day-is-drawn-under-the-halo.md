---
description: A biome is checked in the colours the client shows once its halo is added, never in the colours of its tilesets
trigger: always_on
---

# RULE: A biome is judged in the colours the client shows, its halo added — a middle tone comes out almost white

Why: the client adds over the background a copy of it blurred at 70 % (`Sky::drawHalo`, `HALO_TINT`); the beige sky of Deseret, drawn by day at about (161, 136, 109), showed at about (250, 235, 185), and its birds, whose death flashes in cream, seemed to vanish.

## Rule

1. **Before shipping the tilesets of a biome, compute what a flat area shows**: its colour times `1 + HALO_TINT.a / 255` (1.7), each channel cut at 255. A channel above 150 comes out saturated.
2. **A biome drawn by day divides its tilesets by that gain** (`darken_for_halo.py` of the change that shipped Deseret), so a flat area shows its own colour again; a dark biome (the Pit, the Ashlands) keeps its own.
3. **Look at it in the game against a sprite that flashes**: the cream or white first frame of a death must stand out of the sky.

## Example

- ❌ **Before (wrong)**: `deseret_far.png` exported as the generator drew it → a sky near white in game, the Carrions' death invisible.
- ✅ **After (right)**: each channel of the 15 tilesets divided by 1.7 → the beige of the design in game, and every death seen.
