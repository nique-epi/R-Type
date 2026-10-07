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

### A claim about the agent's environment is measured in that environment

1. **Every statement a brief makes about the environment** ("the build works", "dependencies are installed", "the tests pass on this checkout") **is run once in the exact setup the agent receives** (same worktree, same links, same command) before the brief is sent.
2. **A shortcut that changes the setup is a new environment**: a symlinked dependency folder, a shared build directory, a copied cache. Measure it there; never inherit the result from the folder it was copied from.
3. **A claim found false after launch is corrected at once**: tell every running agent that the claim was false and what was observed (command, exit code, error), and forbid the destructive workaround (deleting or reinstalling a shared folder). Name the setup as the cause only with evidence that tells it apart from a code defect, such as the same command passing once the setup is repaired.

## Example

- ❌ **Before (wrong)**: brief → "two simulations with the same seed produce the same state at tick 100; write the test asserting it" — without checking (enemies shoot using `std::random_device`).
- ✅ **After (right)**: run two simulations → see them diverge at tick 12 → brief → "the simulation is not deterministic today (source: `EnemySpawner` uses `std::random_device`). Make it deterministic by injecting the generator, then assert equality at tick 100."
- ❌ **Before (wrong)**: `docs/node_modules` symlinked into six worktrees, brief → "`npm run build` in `docs/` works directly" → Astro resolves the real path outside the project and every build fails with "No cached compile metadata".
- ✅ **After (right)**: run `npm run build` in one prepared worktree first → it fails with the symlink → `npm ci --prefer-offline` in each worktree, build measured green, then brief.
