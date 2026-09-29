---
description: Facing an instruction made ambiguous by a typo, keep the reading that applies the project rule, never the one that suspends it
trigger: always_on
---

# RULE: Facing an instruction made ambiguous by a typo, keep the reading that SERVES the project rule — never the one that suspends it

## Rule

1. **An ambiguous instruction that could suspend a rule is read in the direction that APPLIES the rule.** Applying the rule for nothing costs a `git fetch`; wrongly suspending it costs a rebase and a force-push.
2. **A misspelled word that changes the meaning is cleared up before acting**, in one line.
3. **Never write an assumption as a fact** in a commit, a PR or `.context/context.md`. An interpretation is stated as such, or confirmed.
4. **`git fetch origin main` before creating any branch**, whatever you think you read: it is read-only and breaks nothing.

## Example

- ❌ **Before (wrong)**: an instruction reading "start from a stale main" (typo for "start from an up-to-date main") → branch created without fetching, based on a `main` one merged PR behind.
- ✅ **After (right)**: `git fetch origin main` (zero cost), and a one-line question if the doubt remains.
