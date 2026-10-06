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
The error log showed the symlink target's path appended to the worktree's path, which pointed at the symlink. The agents were warned while running and told not to delete or reinstall the shared folder. The cause was then confirmed: on one worktree, the same build exited with code 0 once the symlink was replaced by `npm ci --prefer-offline`, and every worktree got the same install.

### Review follow-up
The first version of the rule told the coordinator to "say the failure comes from the setup". A review pointed out that this grants the cause in advance; the rule now asks for the observed result, and for evidence before naming the setup.

## Rule
See `.claude/rules/fix/reasoning/fix-reasoning-delegated-invariant-must-be-computed.md`, section "A claim about the agent's environment is measured in that environment".
