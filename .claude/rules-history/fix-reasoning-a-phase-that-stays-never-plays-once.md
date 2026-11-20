

## Context

Deseret's sheets were written by a script from the design artifact; the bosses' appearance tables were copied as the artist wrote them. In game, the Iron Pilgrim rose from the ground, then vanished while its boss bar stayed; the birds vanished without being hit.

## Mistake

Nine attack phases (the strikes of the Pilgrim, the Widow, the Carrion, the Aeronaut, the Lantern Wisp and the Surveyor) played `once` and stayed on their last frame. The investigation first chased the death table of the boss sheet, a measurement that did not discriminate: the boss was visible or not depending on when the screenshot fell against its first strike.

## Root cause

`AnimationSystem::finish()` destroys an entity whose animation ended with `Playback::Once` and no animation after it, which is right for an effect. `EnemyActionPlayer` gives an action no animation after it when the sheet says `stay`, so a strike played once destroyed the body on the client. The sheet reader accepted the pair, and no sheet of Fosse or the Ashlands used it.

## Rule

1. Before writing `afterwards = "stay"` in an attack phase, write `playback = "hold"` (or `"loop"`) next to it.
2. The client refuses the pair at reading (`InconsistentEntitySheetException`).
3. A body that vanishes on the client while the server still has it is read for a destroy on the client first.

## Example

- ❌ **Before (wrong)**: the Pilgrim's volley strike `once` + `stay` → the locomotive gone after its first volley.
- ✅ **After (right)**: `hold` + `stay`; the Pilgrim stays through volleys, charges and broods.
