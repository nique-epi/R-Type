# RULE: What a screen shows of a state never waits on an earlier message it can do without

## Context

The client presentation of the Bydo Tyrant (`BossPresentation`) builds what the screen lays over the playfield: the WARNING screen and the red edges, timed from the Arriving status, and the bar, built from the statuses of the fight.

## Mistake

The approved plan wrote `look()` to return an empty look until an Arriving status had come. A game screen test that queued a fight status alone saw no bar: the bar depended on a message it does not need.

## Root cause

The look was gated as a whole on the first message of the sequence, assuming it always arrives. A guaranteed message is still lost past its resend window, so the gate turned one lost message into a bar missing for the whole fight.

## Rule

1. Before gating a look on a message, list each part of the look and the message it really needs; compute each part from its own message.
2. Walk the case where the earlier message never arrives, and write what the screen shows then.
3. A test feeds the later message alone and checks that its part shows.

## Example

- ❌ `look()` returned `{}` without an Arriving status.
- ✅ The bar comes from `bossBarAt(fight)`; the WARNING screen and the edges only once the Arriving status came.
