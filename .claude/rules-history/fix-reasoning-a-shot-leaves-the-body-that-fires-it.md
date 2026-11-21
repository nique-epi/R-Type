

## Context

Deseret was played by the user after it was pushed (2026-11-21).

## Mistake

the Surveyor of Deseret, a turret on the ground, spawned its slug on the row of the nearest ship, so the shot came out of the air far above the turret.

## Root cause

The sheets and the scripts were written from the design, and the attack was never watched in the game before the push.

## Rule

1. **Before a script spawns a shot, compute where it appears from the position of the body that fires it** plus its muzzle (`LENS`, `MUZZLE_GAP`), never from the target.
2. **A shot that must reach a target elsewhere flies towards it** from the muzzle (as `aimed_bolt` and `aimed_slug` do); a warning drawn along a row (a sight) only fits a shot that flies along that row from its body.
3. **Look at the attack in the game**: the shot leaves the body on screen.

## Example

- ❌ **Before (wrong)**: `behaviours.spawn("surveyor_shot", { x = e.position.x + LENS.x, y = ship.y })` with a horizontal sight on the row of the ship.
- ✅ **After (right)**: the slug spawned at `e.position + LENS` and piloted by `aimed_slug` towards the ship; the sight removed.
