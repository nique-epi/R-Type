---
description: Every sheet of a body a missile can kill names its own [appearance.death], or the client draws the Bydo explosion
trigger: always_on
---

# RULE: A body a missile can kill draws its own death — without one, the client plays the Bydo explosion

Why: the brood mite, the toxic barrel, the billboard and the vent grille of the Ashlands had health and no `[appearance.death]`; shot down, each showed the Bydo of the shipped explosion.

## Context

The resource ashlands added 37 entity sheets, most of their sprites taken from a design whose mobs each had a death row. The decor that breaks (barrel, billboard, vent grille) and the brood mite were drawn or copied without one.

## Mistake

The sheets were checked by the tests that read every appearance, which accept a sheet without a death. In the game, the user saw a Bydo appear where a barrel or a mite was shot down.

## Root cause

A sheet without `[appearance.death]` is valid: the client falls back on the Bydo explosion of `sprites/effects.png`. Nothing in the tests or the build tells a missing death from a chosen fallback.

## Rule

1. **Before shipping a resource, list its sheets that a missile can kill** (a `[gameplay.health]` on a body of side `enemy` or `obstacle`) **and check each has `[appearance.death]`**, with its no-flash row under it:
   ```bash
   for f in resources/<resource>/entities/*.toml; do grep -q 'gameplay.health' "$f" && ! grep -q 'appearance.death' "$f" && echo "$f"; done
   ```
   It must print nothing.
2. **A body removed by its script** (`behaviours.destroy`) plays no death and needs none; one a missile kills always does.
3. **Look at one death of each new sheet in the game**, not only at the sheet: the fallback is a picture of another enemy, which no test reads.

## Example

- ❌ **Before (wrong)**: `toxic_barrel.toml` with `points = 1` and an idle only → shot, a Bydo flashes where the barrel stood.
- ✅ **After (right)**: the barrel's sheet holds a death of six frames, its silhouette flashing then scattering, and the loop above prints nothing for `resources/ashlands/`.
