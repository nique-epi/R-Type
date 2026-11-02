---
description: A delay stated in ticks, frames or messages is read on a written timeline, never estimated
trigger: always_on
---

# RULE: A delay is counted on a written timeline

## Rule

Before writing a delay in ticks, frames, round trips or messages (in a question, a plan, a doc or a PR), write the timeline step by step ("tick N: the collision is published, then delivered; tick N+1: …") and read the count on it. Never derive it from a shortcut such as "one tick per step".

### How long something stays on screen is read where the draw falls in the loop

1. **Before saying how many frames a screen, an entity or an effect is shown**, write the frame loop in its real order (events, update, changes applied, draw) and place on it where the change is asked for, where it is applied, and where the draw happens.
2. **A change asked for and applied before the draw of the same frame is never drawn**: the screen it removes is shown 0 frames, not one.
3. **Find the phase the request is made in before counting.** A screen asked for from an event is built before the update of that frame, so it updates before its first draw. A screen asked for from an `update()` (a screen that changes screens on a timer or on a network answer) is built after the update phase: it is drawn once before its own first update.

### Each write on the timeline sits in the stage the code runs it in

1. **Before claiming a counter changes its timing, or keeps it, find the stage of every write and every read of it**: `git grep -n 'addWork(' -- <module folders>` names the stage each module works in, and the order of the stages is declared once (`GAME_STAGES`, `SERVER_LOOP_STAGES`, `CLIENT_LOOP_STAGES`). Place each write (the send that resets the counter, the time that is added) and the check that reads it on the tick, in that order.
2. **Never place a write from what it is for**: "the world state is an output, so it goes after the output stage" is a guess; the rules module sends it from the simulation stage.
3. **Write the count for each placement the code holds**, a write before the check and a write after it, and compare them with the tests that pin the timing before saying "nothing changes".

## Example

- ❌ **Before (wrong)**: "with the next-dispatch option, the death is announced 2 ticks after the collision".
- ✅ **After (right)**: timeline written: the collision is published and delivered in tick N, `EntityDestroyed` is published during that delivery and delivered in tick N+1 → "1 tick (16.7 ms) after the collision".
- ❌ **Before (wrong)**: "with one file in the game assets, the loading screen shows for one frame".
- ✅ **After (right)**: PLAY asks for the game from an event, so the loading screen is built before the update; frame 1: `update()` loads the file and asks `replaceAll(Game)`, `applyPendingChanges()` builds the game screen, then `render()` draws it → the loading screen is drawn 0 times; with N files, N − 1 frames.
- ❌ **Before (wrong)**: a plan adds a second sheet to the game assets and says "the loading screen is still drawn 0 times", counted from the frame that loads the file.
- ✅ **After (right)**: the connection screen asks `replaceAll(Game)` from its `update()`; frame N: the loading screen is built after the update, then drawn; frame N+1: it loads the file and the game screen is built before the draw → drawn once (16.7 ms).
- ❌ **Before (wrong)**: a plan adds the real time of each tick to `ConnectedPlayer::sinceSent` at the end of the tick and says the heartbeats keep their timing, counted as if every datagram left after that end; the game sends from the simulation stage, before it, so each send was followed by a whole tick and the heartbeats came every 61 ticks: `SendsAHeartbeatEverySecondWhileNothingElseGoesOut` counted 2 instead of 3.
- ✅ **After (right)**: `addWork` read for every module of a game → every send happens in the simulation stage → the time is added when the tick starts, before any send, and the heartbeat goes once 60 whole ticks passed: every 60 ticks, as the tests require.
