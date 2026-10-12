# RULE: A delay is counted on a written timeline

## Context

Planning the engine event bus, the user had to choose whether an event published while the bus delivers events (the damage subscriber announcing that an entity died) is delivered by the same delivery or by the next one, one tick later.

## Mistake

The option "next delivery" was described as "each link adds a tick (16 ms): the death arrives 2 ticks after the collision". Walking the tick shows 1 tick: the collision is published during the systems of tick N and delivered at the end of tick N, because it was already queued; the death, published during that delivery, is delivered at the end of tick N+1.

## Root cause

The count came from the shortcut "one tick per link", applied to a two-link chain (collision → damage → death), without noticing that the first link is delivered in the tick it is published.

## Rule

Before writing a delay in ticks, frames, round trips or messages, write the timeline step by step and read the count on it. Never derive it from a shortcut such as "one tick per step".

## Example

- ❌ **Before (wrong)**: "the death is announced 2 ticks after the collision".
- ✅ **After (right)**: "tick N: collision published, then delivered, death published; tick N+1: death delivered" → "1 tick (16.7 ms) after the collision".
