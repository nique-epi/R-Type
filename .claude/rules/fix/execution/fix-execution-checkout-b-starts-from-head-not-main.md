---
description: git checkout -b starts from HEAD — always name origin/main as the starting point and check the topology after creation and after push
trigger: always_on
---

# RULE: `git checkout -b` starts from HEAD, not from `main` — name the starting point and check the topology

## Rule

1. **Always name the starting point explicitly**, on the fresh remote ref:
   ```bash
   git fetch origin main
   git checkout -b <type>/<slug> origin/main
   ```
   `origin/main` depends neither on HEAD nor on the state of the local `main`.
2. **Check the topology right after creation**, before any commit:
   ```bash
   git log --oneline origin/main..HEAD   # must be empty on a fresh branch
   ```
3. **Push by creating its own upstream ref**: `git push -u origin <type>/<slug>` (a branch created from `origin/main` tracks it by default; a bare `git push` would target `main`).
4. **After the push, check the PR, not the command**:
   ```bash
   gh pr view <n> --json state,mergeable,commits
   ```
   `MERGEABLE` and the expected number of commits.
5. **Repair**: never rebase/force-push from the user's working tree; set up a `git worktree add --detach <tmp> origin/main`, cherry-pick the legitimate commits there, `push --force-with-lease`, remove the worktree.

## Example

- ❌ **Before (wrong)**: on `ci/build-and-test`, `git checkout -b feat/logger` → the branch carries the CI commits, PR with stray commits.
- ✅ **After (right)**: `git checkout -b feat/logger origin/main` → `git log --oneline origin/main..HEAD` empty → commit → `git push -u origin feat/logger` → `gh pr view`: 1 commit.
