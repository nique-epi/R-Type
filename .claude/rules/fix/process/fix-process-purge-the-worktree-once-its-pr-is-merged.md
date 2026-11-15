---
description: Once the pull request of a worktree is merged, remove the worktree, its build folder and its local branch; ask first when in doubt
trigger: always_on
---

# RULE: A worktree is purged once its pull request is merged — and the session is asked first when in doubt

Why: the user asked for it; every worktree keeps a checkout and a build folder of several gigabytes, and merged ones piled up on the disk.

## Rule

1. **When a pull request I worked on in a worktree is merged, purge that worktree in the same session**: the worktree, its build folders (in the worktree or in the scratchpad) and its local branch.
2. **Prove the merge before purging**, in the call before the purge:
   ```bash
   gh pr view <n> --json state,headRefOid --jq '.state + " " + .headRefOid'
   git -C <worktree> rev-parse HEAD
   git -C <worktree> status --porcelain
   ```
   `MERGED`, the same commit on both lines, and an empty status. The repository squashes its merges, so `git branch -d` refuses the branch: `git branch -D` is allowed only after this proof.
3. **Purge with git, never with `rm -rf` alone**: `git worktree remove <worktree>`, then `git branch -D <branch>`, then `git worktree prune`. No `--force` on `git worktree remove`: a refusal means something is left in it, and it is a reason to ask. Then check the disk, not only the index (`git worktree list` **and** `ls` of the folder, `fix-reasoning-check-the-delivered-artifact-not-its-footprint.md`).
4. **Ask in the session before purging when any doubt remains**: the discussion about the pull request is not finished (a follow-up on the same branch, a question still open, a review to answer), the status is not empty, the worktree holds commits the pull request does not, or the merge was not read with point 2. One line: which worktree, which pull request, what makes me doubt; purge only on the user's yes.
5. **Purge only what I created**: never the main checkout, never a worktree another session or the user works in (files changing under me, work I did not make). A worktree I did not make is listed for the user, not removed.

## Example

- ❌ **Before (wrong)**: twenty worktrees under `.claude/worktrees/` and the scratchpad, most of them for merged pull requests, each with its `build/`, left on the disk after the merge.
- ✅ **After (right)**: #361 merged → `gh pr view 361` reads `MERGED` and the tip of the worktree → status empty → `git worktree remove`, `git branch -D fix/master-tests-bind-any-port`, `git worktree prune`, folder gone from `ls`.
- ✅ **After (right, in doubt)**: #356 merged, but the user just asked for another change to the destination screen → one question in the session, whether to purge `feat/destination-screen` or keep it for the next change; nothing removed before the answer.
