# History: an invariant to be asserted must be computed before being written in a brief

The rule was imported when R-Type started; its original incident belongs to another repository.

## Update (2026-10-06) — a claim about the agent's environment is measured in that environment

### Context
To test a documentation skill, six git worktrees replayed three past features, and six subagents (with and without the skill) were asked to document them and build the documentation site.

### Mistake
To save six installs, `docs/node_modules` was symlinked from the main checkout into each worktree, and the brief stated that `npm run build` in `docs/` "works directly". It had only been run in the main checkout. In the worktrees, Astro resolved the symlink to its real path outside the project and every build failed with "No cached compile metadata".

### Root cause
The build was measured in one environment and the claim was handed over for another. The symlink changed the environment, so the earlier measurement no longer said anything about it.

### Correction
The agents were warned while running and told not to delete or reinstall the shared folder; each symlink was replaced by `npm ci --prefer-offline`, after checking on one worktree that the build then exits with code 0.

## Rule
See `.claude/rules/fix/reasoning/fix-reasoning-delegated-invariant-must-be-computed.md`, section "A claim about the agent's environment is measured in that environment".
