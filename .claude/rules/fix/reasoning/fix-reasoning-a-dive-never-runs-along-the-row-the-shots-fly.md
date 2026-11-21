---
description: A mob that dives at a ship starts its dive near the ship and off its row, never from far ahead along the row the ship fires on
trigger: always_on
---

# RULE: A dive at a ship starts near it and off its row — from far ahead it runs into the shots

Why: the Carrion dived from x 1700 at a ship near x 200; its dive was flat, along the row the ship fires on without stopping, and it died in two hits far from the ship, as if it vanished.

## Rule

1. **Before writing an attack that heads for the position of a ship, compute its heading from where it starts**: a start far ahead gives a heading almost along the row of the ship, which is the row of its shots.
2. **Start such an attack only with the ship within a reach ahead and away from the row of the body** (`reach`, `drop` of `carrion.toml`), so it cuts the stream of shots across instead of flying along it.
3. **When a body is said to vanish, find how it left before changing its look**: a probe that records, for each body, its last place and phase, and on `entity_destroyed` whether it was `killed`. `DamageRules` is the only place that kills: a killed body was shot.

## Example

- ❌ **Before (wrong)**: `fire_from_x = 1700` → heading (-1.00, -0.03) over 1484 units, the Carrion killed by the autofire 1400 units from the ship.
- ✅ **After (right)**: `reach = 640`, `drop = 120` → it crosses the screen, then slants down on the ship, which it hits (`P1 HIT`) unless the ship shoots it.
