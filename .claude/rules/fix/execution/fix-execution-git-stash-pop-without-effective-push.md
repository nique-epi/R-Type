---
description: Never chain git stash push and git stash pop — a push that saves nothing pops an older entry; prefer not stashing
trigger: always_on
---

# RULE: Never chain `git stash push` and `git stash pop` — a push that saves nothing pops the entry from BEFORE

## Rule

1. **Prefer not stashing at all.** The stack is shared by all worktrees: what you pop is not necessarily yours. To split a commit, use the index (`git add -- <files>` then `git commit`, twice). To change base, `git rebase origin/main` or `git merge --ff-only` fail cleanly instead of destroying.
2. **If a stash is unavoidable**, never chain `push` and `pop` in a compound command: run `git stash push -m "<name>"`, **read its output**, check with `git stash list` that `stash@{0}` carries that name, and only then pop.
3. **Check the stack before any pop**: `git stash list`.
4. **After a conflicting `pop`**: `git reset --hard <ref>` undoes the pop and Git keeps the entry. Restored untracked files survive the reset: identify them (`git stash show --include-untracked --name-only stash@{0}`) and remove them one by one, never with a blind `git clean -fd`.
5. **Never `2>/dev/null || true`** on a git command that writes.

## Example

- ❌ **Before (wrong)**: `git stash push README.md && git reset --hard origin/main && git stash pop` → the push saves nothing, the pop restores an old entry, conflicts.
- ✅ **After (right)**: commit the work on its branch, then `git rebase origin/main` — no stack, no risk.
