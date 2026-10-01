---
description: Every git command that writes history rereads the current branch in the same call; never a mutating git command behind an unrelated fallible command
trigger: always_on
---

# RULE: Every git command that WRITES history rereads the current branch in the SAME call

## Rule

1. **Every git command that writes history** — `commit`, `commit --amend`, `rebase`, `reset`, `cherry-pick`, `merge`, `push` — is preceded, **in the same call**, by a read of the current branch, and only runs if it matches:
   ```sh
   [ "$(git branch --show-current)" = "feat/packet-reader" ] || { echo "WRONG BRANCH"; exit 1; }
   git commit --amend --no-edit
   ```
2. **Never chain a mutating git command behind a fallible command on a separate line** (heredoc, script, `cd`) relying on an `&&` placed elsewhere: `&&` only protects what is on its left in the same command. Split into two calls, read the result of the first, then run the second.
3. **Prefer `git -C <dir>`** over `cd <dir>` followed by a command: it depends on no current directory.
4. **Never `--amend` with nothing staged** unless explicitly intended: check `git status --short` first.
5. **When `git log` does not show what you expect, read `git reflog`** before any other command.
6. **Push without checking out** when the working tree belongs to someone else: `git push origin my-branch:my-branch`.
7. **Repairing your own breakage ≠ overwriting someone else's work**: prove equivalence before realigning a ref, otherwise ask.

## Example

- ❌ **Before (wrong)**:
  ```sh
  python3 fix_includes.py
  git add -A && git commit --amend --no-edit
  ```
  → the script fails, the amend runs anyway, on a branch that changed in the meantime.
- ✅ **After (right)**: one call for the script, whose exit code is read; then a second call that checks the branch and the index before amending.
