# RULE: A delay is counted on a written timeline

## Context

Planning the engine event bus, the user had to choose whether an event published while the bus delivers events (the damage subscriber announcing that an entity died) is delivered by the same delivery or by the next one, one tick later.

## Mistake

The option "next delivery" was described as "each link adds a tick (16 ms): the death arrives 2 ticks after the collision". Walking the tick shows 1 tick: the collision is published during the systems of tick N and delivered at the end of tick N, because it was already queued; the death, published during that delivery, is delivered at the end of tick N+1.

## Root cause

The count came from the shortcut "one tick per link", applied to a two-link chain (collision → damage → death), without noticing that the first link is delivered in the tick it is published.

## Update (2026-10-12) — how long a screen stays on screen

### Context
Explaining the client screen stack of another pull request (`ScreenStack`, `LoadingScreen`, `Application`), the user asked what happens once the game sprite sheet is added to the assets of a game.

### Mistake
The answer said the loading screen would show "for one frame, the time to read that file". Walking the loop of `Application::run()` gives 0 frames: in frame 1, `LoadingScreen::update()` reads the only file and asks `replaceAll(Game)`, `applyPendingChanges()` replaces it with the game screen, and only then `render()` draws. The pull request's own description already said the loading screen "is never seen" when nothing is left to load.

### Root cause
The count came from the shortcut "one file per frame, so one frame on screen", without placing the draw in the loop order: the change is applied between the update and the draw of the same frame.

## Rule

Before writing a delay in ticks, frames, round trips or messages, write the timeline step by step and read the count on it. Never derive it from a shortcut such as "one tick per step".

## Example

- ❌ **Before (wrong)**: "the death is announced 2 ticks after the collision".
- ✅ **After (right)**: "tick N: collision published, then delivered, death published; tick N+1: death delivered" → "1 tick (16.7 ms) after the collision".
