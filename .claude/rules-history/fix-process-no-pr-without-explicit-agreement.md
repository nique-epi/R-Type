# History: no pull request without the user's agreement

## Context
While planning the client resource manager, the user corrected the platform code of the plan. `core/errors-learning.md` requires a corrective rule for every correction, and the user's global instructions then said to open a PR as soon as a piece of work is ready.

## Mistake
The corrective rule was committed, its branch pushed and a draft PR opened in the same turn, without asking. The user said to stop opening PRs without their agreement.

## Root cause
Two written instructions ("create the rule, never postponed" and "open the PR without waiting") were read as an authorization to publish. Neither says the user agreed to that particular PR.

## Rule
See `.claude/rules/fix/process/fix-process-no-pr-without-explicit-agreement.md`. The user's global instructions were updated the same day: a finished feature is reported right away, and its branch is pushed and its PR opened only after the user's go.

## Example
- ❌ **Before (wrong)**: rule written, committed, pushed and opened as a PR in one turn.
- ✅ **After (right)**: rule committed locally, then "push it and open the PR?" in the answer.
