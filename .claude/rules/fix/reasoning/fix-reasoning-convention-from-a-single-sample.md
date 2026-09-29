---
description: A convention is derived from at least three representative instances, including the most recent — never from a single example
trigger: always_on
---

# RULE: A convention is derived from a REPRESENTATIVE sample (≥ 3, including the most recent) — never from a single example

## Rule

1. **Before codifying or applying a convention** (naming, folder layout, commit format, PR template, shape of an ECS system), **read at least three complete instances**, including the most recent, and check that they converge. If they diverge, the divergence *is* the information: find what distinguishes the cases before writing anything.
2. **An isolated instance that differs from the others is a variant to understand, never the norm.**
3. **When the user clearly has a format in mind** ("follow what's already in place"), **show them the chosen skeleton before applying it at scale**.
4. **Never write an unmeasured coverage claim.** "Derived from the history" requires having read the history; otherwise write what you actually read.
5. Neighbors: `fix-reasoning-never-assume-unread-file-content.md` covers "I did not read"; `fix-reasoning-delegated-invariant-must-be-computed.md` covers "I read but paraphrased wrong"; this one covers **"I read ONE case and generalized it"**.

## Example

- ❌ **Before (wrong)**: one commit has a `feat(server): …` scope → I make a scope mandatory on every commit.
- ✅ **After (right)**: `git log --format=%s -20` → most have no scope → the scope stays optional, and the exception is understood as such.
