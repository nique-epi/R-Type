---
description: Prove the premise (is the requested state the applied state?) before fixing behavior that derives from it
trigger: always_on
---

# RULE: Verify the premise before fixing downstream

## Rule

1. **Before fixing behavior derived from a state, prove the state.** The first log to add shows side by side **what is requested** and **what is actually applied** (position sent vs position rendered, tick requested vs tick simulated). A mismatch between the two ends the debate.
2. **Read your own logs as an adversary.** A line written to confirm one hypothesis often contains the refutation of another: for each displayed field, ask what it would say if the hypothesis were wrong.
3. **A symptom that systematically points at the first element** (first entity, first player, index 0, id 0) is almost always an **origin defect**: something never left its initial value.
4. **When two levers target the same thing without being able to contradict each other**, a fix that is right under both hypotheses is better than one right under only one.

## Example

- ❌ **Before (wrong)**: "shots come out of the wrong ship" → fix the cannon offset, then the rotation, then the interpolation — three rounds.
- ✅ **After (right)**: log `requested owner=3, spawned owner=0` → the owner is never set when the projectile is created → fix the origin, then check whether anything remains to fix downstream.
