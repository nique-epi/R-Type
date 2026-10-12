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

## Example

- ❌ **Before (wrong)**: "with the next-dispatch option, the death is announced 2 ticks after the collision".
- ✅ **After (right)**: timeline written: the collision is published and delivered in tick N, `EntityDestroyed` is published during that delivery and delivered in tick N+1 → "1 tick (16.7 ms) after the collision".
- ❌ **Before (wrong)**: "with one file in the game assets, the loading screen shows for one frame".
- ✅ **After (right)**: frame 1: `update()` loads the file and asks `replaceAll(Game)`, `applyPendingChanges()` builds the game screen, then `render()` draws it → the loading screen is drawn 0 times; with N files, N − 1 frames.
