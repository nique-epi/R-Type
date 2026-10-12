---
description: A delay stated in ticks, frames or messages is read on a written timeline, never estimated
trigger: always_on
---

# RULE: A delay is counted on a written timeline

## Rule

Before writing a delay in ticks, frames, round trips or messages (in a question, a plan, a doc or a PR), write the timeline step by step ("tick N: the collision is published, then delivered; tick N+1: …") and read the count on it. Never derive it from a shortcut such as "one tick per step".

## Example

- ❌ **Before (wrong)**: "with the next-dispatch option, the death is announced 2 ticks after the collision".
- ✅ **After (right)**: timeline written: the collision is published and delivered in tick N, `EntityDestroyed` is published during that delivery and delivered in tick N+1 → "1 tick (16.7 ms) after the collision".
