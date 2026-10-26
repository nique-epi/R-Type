# History: check the index before committing

Rule imported from another project when R-Type started: its original incident belongs to another repository.

## Update (2026-10-16) — a path staged earlier rides along with the next commit

### Context
On the connection branch, the client commit was prepared first: `git rm -r` of the stand-in connection and its test staged four deletions. A refactor of the datagram header check, in other files, had to be committed before it.

### Mistake
`git add <the five refactor files> && git diff --cached --no-renames --name-status && git commit -F -` ran in one call. The index listing showed the four `D` lines, but the commit was already made: the refactor commit deleted the stand-in. It was undone with `git reset --soft HEAD~1` and `git restore --staged` of the four paths, then made again.

### Root cause
`git add` was read as "the index now holds these paths", when it only adds to what is already staged; and the index was printed in the same call as the commit, so reading it could not stop anything.

### Rule
Read the index in one call and commit in the next; unstage a `git rm` meant for a later commit before making an earlier one.

## Update (22 October) — a stray `git add -N -- .` in a staging call

### Context
Splitting the visual effects of the client into five commits from one working tree, which also held local rule files under `.claude/` that belong to no commit.

### Mistake
The staging call of the third commit added its paths, then ran `git add -N -- . 2>/dev/null`, a leftover with no purpose there: every untracked file, the five local rule files included, became intent-to-add. `git diff --cached` did not show them; `git status --porcelain` did, as ` A`, before the commit.

### Root cause
A command not tied to the commit's paths was typed into the staging call, with its errors silenced, and the cached diff alone was trusted to show the index.

### Rule
A staging call names the paths of its commit only, never silences a git command that writes, and the index check reads `git status --porcelain` for intent-to-add entries.

## Update (25 October 2026) — A small commit staged, printed and committed in one call

The split of the grains of a charge, two files, was staged, its index printed and committed in the same call; the index held exactly the two files, so the commit was right by luck. New subsection: even a small commit is made in the call after the one that prints its index.
