---
description: An ambiguous instruction where one reading inverts existing behavior is confirmed before implementing
trigger: always_on
---

# RULE: An ambiguous instruction that could invert existing behavior is confirmed before implementing

## Rule

Facing an instruction where one reading **inverts existing behavior** and the other **confirms** it:

1. **List the clauses and look for a reading that makes all of them true.** A clause you have to call a "slip" to save a reading is the signal you hold the wrong one.
2. **Only if no complete reading exists**, the start of the sentence wins: if the instruction opens with a clear imperative, the ambiguous clause is read in its direction.
3. **If the ambiguity persists, ask the one-line question before coding.** Confirming costs ten seconds; wrongly inverting costs a PR to undo.
4. **Never invert shipped behavior on the sole basis of an ambiguous sentence**: what exists is a signal of intent.
5. **A reading that removes a deliberately shipped mechanism is confirmed before the first `git rm`.** The reading that keeps it is the default.
6. **Raise an ambiguity before delivering, not after**: a doubt stated on top of an implementation already written does not redeem the code.
7. **Cross-cutting data is not decided locally**: before changing a direction, order or convention (Y axis, speed unit, byte order), look for who else reads it.

## Example

- ❌ **Before (wrong)**: "enemies must not shoot when they are off screen, except the boss" → read as "only the boss shoots" → every on-screen enemy stops shooting.
- ✅ **After (right)**: reading that makes every clause true: "off screen, nobody shoots except the boss; on screen, nothing changes" — and, at the slightest doubt, a one-line question.
