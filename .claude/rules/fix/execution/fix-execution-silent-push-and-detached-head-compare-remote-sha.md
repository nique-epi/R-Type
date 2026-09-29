---
description: A push is never claimed from its exit code — compare the remote SHA with local HEAD and the PR head; check HEAD is not detached before committing
trigger: always_on
---

# RULE: A push is never claimed from its exit code — compare the remote SHA with the local HEAD

## Rule

1. **Before each `git commit`, assert the branch**: `git branch --show-current` must return the expected branch. An **empty** output is a detached HEAD: do not commit, diagnose (`git reflog show HEAD -5`) and reattach first.
2. **Never `-q` on a push whose result you will report**, never `&& echo pushed`: read the `old..new  branch -> branch` line. "Everything up-to-date" after a local commit is an **error**.
3. **After the push, compare the SHAs**:
   ```bash
   git rev-parse HEAD
   git ls-remote --heads origin <branch> | cut -f1
   gh pr view <n> --json headRefOid --jq .headRefOid
   ```
   All three must match. GitHub can take a few seconds to update the PR head: re-query before concluding.
4. **Repair**, when the detached commits are all yours: `git branch -f <branch> HEAD`, `git checkout <branch>`, verbose push, then point 3. If a foreign commit is among them: STOP, ask.

## Example

- ❌ **Before (wrong)**: `git push -q && echo pushed` → "pushed", PR unchanged, blamed on "GitHub recomputing".
- ✅ **After (right)**: `git branch --show-current` → empty → detached HEAD → reattach the branch → push → `ls-remote` = `rev-parse HEAD` = `headRefOid`.
