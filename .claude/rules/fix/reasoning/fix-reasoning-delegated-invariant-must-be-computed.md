---
description: An invariant handed to a test or a subagent to assert must be computed before being written in the brief
trigger: always_on
---

# RULE: An invariant to be asserted must be COMPUTED before being written in a brief — never paraphrased from memory

## Rule

1. **Every invariant a brief asks to ASSERT in a test must first be COMPUTED** — throwaway program, exploratory test, run on the real code — never derived from memory or copied from an earlier reasoning. If I have `EXPECT_EQ(a, b)` written, I have run `a` and `b` and I know their values.
2. **A design slogan is not a test specification.** "Serialization is symmetric", "the tick is deterministic", "it's idempotent" are summaries for a human. Before having them asserted, translate them into exact mechanics (which function, which input, which output) and check that the translation holds.
3. **If the brief cannot carry the computed value, it carries the order to compute it**: "derive the invariant from the code and measure it before asserting", never "the invariant is X, assert X". An agent given a false invariant as a fact will force it instead of questioning it.
4. **When an agent disputes an invariant of my brief, measure before deciding.** Never reassert the brief by authority.

## Example

- ❌ **Before (wrong)**: brief → "two simulations with the same seed produce the same state at tick 100; write the test asserting it" — without checking (enemies shoot using `std::random_device`).
- ✅ **After (right)**: run two simulations → see them diverge at tick 12 → brief → "the simulation is not deterministic today (source: `EnemySpawner` uses `std::random_device`). Make it deterministic by injecting the generator, then assert equality at tick 100."
