---
description: Never open a pull request or push a new branch without the user's explicit agreement in the conversation, corrective rules included
trigger: always_on
---

# RULE: No pull request and no newly pushed branch without the user's explicit agreement

## Rule

1. **Before `gh pr create`, and before the first `git push` of a branch, ask** in one line what would go out (branch, commits, PR title), then wait for an explicit yes in the conversation.
2. **Corrective rules from `core/errors-learning.md` are no exception**: the rule is written and committed on its own branch right away, then pushed only once the user agrees.
3. **An agreement covers what was named**: that branch, that PR. The next PR asks again.
4. **Pushing to a PR the user agreed to** (a review fix, a CI fix through Auto-fix) needs no new agreement.
5. **A PR opened without agreement is reported at the top of the answer** and left as it is until the user says to keep or close it.

## Example

- ❌ **Before (wrong)**: a corrective rule is written, committed, pushed and opened as a draft PR in the same turn, without asking.
- ✅ **After (right)**: the rule is committed on `chore/rule-<slug>`; the answer says "rule committed locally on `chore/rule-<slug>`: push it and open the PR?" and waits.
