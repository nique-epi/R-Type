---
description: An attack phase that stays on its last frame plays loop or hold, never once — the client destroys a body whose animation played once with nothing after it
trigger: always_on
---

# RULE: An attack phase that stays on its last frame plays `loop` or `hold`, never `once`

Why: `AnimationSystem` destroys an entity whose animation played `once` with nothing after it; a strike written `playback = "once"` with `afterwards = "stay"` took the Iron Pilgrim, the Old Widow and the Carrions off the client's screen while the server kept them alive.

## Rule

1. **Before writing `afterwards = "stay"` in an attack phase, write `playback = "hold"` (or `"loop"`) next to it.** `once` is for a phase that goes back to idle (`afterwards = "idle"`, or left out) and for an effect, which must vanish.
2. **The client refuses the pair**: `EntityAppearances` throws `InconsistentEntitySheetException` on `once` with `stay`, so `ReadsEverySheetOf…Resource` fails on a sheet that holds it. Run that test of the resource before anyone plays it.
3. **A body that vanishes on the client while the server still has it** (its boss bar shown, its hits counted) **is read for a destroy on the client first**: log what removes it (`AnimationSystem`, `removeUnlistedAt`, `EntityDestroyed`) before reasoning on what draws it.

## Example

- ❌ **Before (wrong)**: `[appearance.attack_0.strike]` of the Iron Pilgrim with `playback = "once"` and `afterwards = "stay"` → after its first volley, the boss bar stays and the locomotive is gone.
- ✅ **After (right)**: `playback = "hold"`, `afterwards = "stay"`; the Pilgrim stays through its volleys, charges and broods, and a sheet with the old pair is refused at reading.
