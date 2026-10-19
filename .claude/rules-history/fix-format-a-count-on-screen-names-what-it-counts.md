---
description: A number shown to the player names what it counts, checked against the rule the screen reports, so it never reads as a requirement
trigger: always_on
---

# RULE: A count on screen names what it counts, and is read against the rule the screen reports

## Context

Design of the waiting room (RTY-107) before any plan or code: three proposals on a design canvas, each with a status line, a pilot count and four example states. The start rule was known from the protocol (`GameStarted` "when every connected player is ready") and from RTY-57, and is implemented in PR #86 (`GameInstance::everyPlayerIsReady`: at least one connected player, and all of them ready).

## Mistake

The status line said `WAITING FOR 2 PILOTS` when two connected pilots were not ready, the header said `3/4`, and the "all ready" example state showed four players. The user read the status as "two more players must join" and asked whether a game can start with two players: it can, and even with one.

## Root cause

The text was written from the data it was computed from (the list of connected players not ready), never read as a player who does not know the rule. "Waiting for N" and a fraction both name a target to reach; the rule has none. The example state reinforced it by showing the room full.

## Rule

1. **Before writing a screen text that holds a number or a fraction** (`WAITING FOR 2 PILOTS`, `3/4`, `2 LEFT`), write every way a player who does not know the rule could read it: who is counted, and what the number seems to ask for.
2. **Check each reading against the rule the screen reports**, read in the code or the protocol (who starts the game, what ends a wait). A reading that contradicts the rule is a defect, even when another reading is right.
3. **Name the counted subject and its state in the text**: `2 PILOTS NOT READY`, `3 PILOTS`. Never a bare "waiting for N", which reads as players missing, and never a denominator (`3/4`) when nothing requires reaching it: a capacity is shown by the free places, not as a target.
4. **The example states of a mockup show the rule's edge, not its maximum**: if a game starts with fewer players than the room holds, a state shows it starting with fewer.

## Example

- ❌ **Before (wrong)**: the waiting room says `WAITING FOR 2 PILOTS` (two connected pilots not ready) and `3/4` above the list, and its "all ready" state shows four players; the server starts a game as soon as every connected player is ready, from 1 to 4, and the text reads as "two more players must join".
- ✅ **After (right)**: `2 PILOTS NOT READY`, `CAPTAIN_STARDUST IS NOT READY`, `3 PILOTS` above the list, and the "all ready" state starts with three players and one open slot.
