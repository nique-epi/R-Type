---
description: When a screen animates the reveal of an outcome, every part that shows the outcome waits for the phase that ends the reveal, never for the data alone
trigger: always_on
---

# RULE: An outcome shows once its reveal ends — every part that shows it waits for the phase, never for the data alone

Why: the roulette roster wrote `+900` as soon as the master sent the payouts, which it does while the wheel still turns, so the player read the result before the wheel stopped.

## Rule

1. **When a screen animates the reveal of an outcome** (a wheel that stops, a card that turns, a countdown), **find when the data of the outcome reaches the client** and when the reveal ends. A server often sends the outcome early, so the animation can land on it.
2. **List every part of the screen that shows the outcome or a value derived from it** (the number, a gain, a banner, a color, a sound, a balance), and gate each one on the phase that ends the reveal (`RoulettePhase::Result`), never on `has_value()` of the data.
3. **The test double carries the outcome before the reveal ends, as the real server does**: a fixture of the turning wheel holds the number and the payouts (`drawnSpinningRoom()`), and a test checks that nothing of the outcome shows then.

## Example

- ❌ **Before (wrong)**: `rowOf()` wrote the gain when `player.payout.has_value()`; `spinningRoom()` held no payout, so the tests passed while the game showed `+900` during the spin.
- ✅ **After (right)**: `rowOf()` writes the gain only when `resultShown(reading)`; `TheRosterHidesTheGainWhileTheWheelTurns` reads a turning wheel whose payouts are known and expects the stake.
