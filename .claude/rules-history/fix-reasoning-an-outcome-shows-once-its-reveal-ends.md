# RULE: An outcome shows once its reveal ends — every part that shows it waits for the phase, never for the data alone

## Context

The roulette table of the casino turns its wheel once the bets close, and stops it on the number drawn when the phase `Result` begins. The master draws the number as the bets close and sends, in every state of the room during the spin, the `winningNumber` and the `payout` of each player seated (`fillRoomState()` in `src/master/Casino/Services/CasinoService.cpp`): the client needs the number to turn the wheel towards it.

## Mistake

The plaque of the number, the status line, the jackpot banner and its sound all waited for `RoulettePhase::Result`. The roster of the players, at the top left, did not: `rowOf()` in `RouletteTexts.cpp` wrote the gain as soon as `player.payout.has_value()`. During the whole spin, the row of a winner read `+900` in green, and the player knew the result before the wheel stopped. The fixture `spinningRoom()` held no payout, so no test could see it.

## Root cause

The gain was gated on the presence of the data, read as "the round paid", while the data arrives before the reveal ends. The test double was tamer than the master: it never sent the payouts during the spin.

## Rule

See `.claude/rules/fix/reasoning/fix-reasoning-an-outcome-shows-once-its-reveal-ends.md`.
