

## Context

Deseret was played by the user after it was pushed (2026-11-21).

## Mistake

the stick of dynamite of Deseret spawned a blast meant to kill it; the blast hit the layer `enemy`, the stick lay in `enemy_missile`, so the stick stayed on the ground for good and the level waited for it.

## Root cause

The sheets and the scripts were written from the design, and the attack was never watched in the game before the push.

## Rule

1. **Before a script spawns a body meant to kill its spawner** (a blast, a burst), read the `hits` of the spawned sheet and the `layer` of the spawner: the layer is in the list, or the kill never happens.
2. **A body that waits to be killed has a way out of its own**: after the time its killer lasts, it destroys itself, so a miss never leaves a body that holds the level.
3. **Play the stage with bots and read whether its level goes on**: a level that stops moving with enemies on screen is the symptom.

## Example

- ❌ **Before (wrong)**: `aeronaut_stick` in `layer = "enemy_missile"`, its `dynamite_blast` with `hits = ["enemy"]` → sticks standing on the ground, the level stuck.
- ✅ **After (right)**: the stick in `layer = "enemy"`, and gone `LINGER_AFTER_BURST_MS` after its burst if the blast still missed it.
