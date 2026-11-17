# History: purge the worktree once its pull request is merged

## Context
Sessions work in worktrees under `.claude/worktrees/` and in their scratchpads, each with a checkout and a build folder. On 2026-11-15, `git worktree list` showed eighteen worktrees besides the main checkout, several of them on branches whose pull request was already merged (#350, #354, #361).

## Mistake
Nothing removed a worktree once its pull request was merged, so merged checkouts and their builds kept their disk space.

## Root cause
No rule said when a worktree ends. The work stopped at the merge, and the worktree was left to the user.

## Rule
See `.claude/rules/fix/process/fix-process-purge-the-worktree-once-its-pr-is-merged.md`. The user's instruction: purge the worktree once the pull request is merged, to free the space; when in doubt, or when the discussion is not finished, ask in the session before purging.
