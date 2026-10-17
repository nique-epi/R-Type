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
