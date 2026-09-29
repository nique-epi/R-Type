---
description: Create the work branch from origin/main before the first commit — one feature = one branch = one PR
trigger: always_on
---

# RULE: Create the branch BEFORE the first commit — the checked-out branch is never an acceptable default

## Rule

**Before the FIRST `git commit` of a unit of work — not before the push, not before the PR:**

1. **Name the work of the upcoming commit and compare it to the current branch.** `git branch --show-current`: if the name does not describe the work, **do not commit on it**. A feature branch only takes its feature; a fix found along the way goes on its own branch. Never commit directly on `main`.
2. **Create the branch from the fresh remote ref** (see `fix-execution-checkout-b-starts-from-head-not-main.md`), named `<type>/<slug>` like in the history (`chore/build-system`, `ci/build-and-test`).
3. **Re-check the branch state at commit time**, not at the start of the session: a branch whose PR is merged and whose remote ref is deleted is **dead** — any commit put on it is orphaned.
4. **Several unrelated fixes = several branches.** A branch costs nothing; untangling afterwards costs one cherry-pick per commit.
5. **Repair**, if commits are already misplaced: never rebase or force-push the user's working tree. Set up `git worktree add --detach <tmp> origin/main`, cherry-pick the commits onto new branches there, push, remove the worktree.
6. **Never delete the misused branch before its commits are replanted and pushed elsewhere** — it is the only copy.

## Example

- ❌ **Before (wrong)**: on `feat/entity-registry`, fix a CI bug → commit on that branch → the registry PR ships an unrelated CI change.
- ✅ **After (right)**: `git fetch origin main && git checkout -b ci/fix-cache origin/main` → `git log --oneline origin/main..HEAD` empty → commit, push, separate PR.
