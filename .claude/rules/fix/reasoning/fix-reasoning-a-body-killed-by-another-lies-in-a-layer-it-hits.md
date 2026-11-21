---
description: A body that counts on another body to kill it lies in a layer that body hits, and has a way out of its own
trigger: always_on
---

# RULE: A body that counts on another to kill it lies in a layer that other body hits

Why: the stick of dynamite of Deseret spawned a blast meant to kill it; the blast hit the layer `enemy`, the stick lay in `enemy_missile`, so the stick stayed on the ground for good and the level waited for it.

## Rule

1. **Before a script spawns a body meant to kill its spawner** (a blast, a burst), read the `hits` of the spawned sheet and the `layer` of the spawner: the layer is in the list, or the kill never happens.
2. **A body that waits to be killed has a way out of its own**: after the time its killer lasts, it destroys itself, so a miss never leaves a body that holds the level.
3. **Play the stage with bots and read whether its level goes on**: a level that stops moving with enemies on screen is the symptom.

### A body that leaves by another edge than the left is removed by its script

1. **Only the left edge removes an enemy**: a body that rolls, flies or is pushed out by the right, the top or the bottom stays alive out of sight, and `level.enemies()` counts it for good. Its script destroys it past that edge (`GONE_RIGHT` of `cart_roll`).
2. **A blast spawned past the right edge is a missile there, removed at once**: a body that counts on its own blast is gone before it bursts out of the screen.

### A boss sheet keeps the keys its precedents share

1. **Before writing the `[gameplay]` of a boss, list the keys every boss sheet of the repository holds** (`hits = ["player"]`, `survives_contact`, `stays_outside`): a key they all hold is part of being a boss, and a sheet without it is the defect. The Iron Pilgrim without `hits` let ships fly through its body.

## Example

- ❌ **Before (wrong)**: `aeronaut_stick` in `layer = "enemy_missile"`, its `dynamite_blast` with `hits = ["enemy"]` → sticks standing on the ground, the level stuck.
- ✅ **After (right)**: the stick in `layer = "enemy"`, and gone `LINGER_AFTER_BURST_MS` after its burst if the blast still missed it.
