---
description: "A new header takes a file name no other header of the repository has: the include folders are flat, so two headers of one name resolve to whichever folder comes first"
trigger: always_on
---

# RULE: A new header takes a name no other header of the repository has — the include folders are flat

Why: a plan added `src/network/Protocol/EncodedMessage.hpp` while `src/server/Replication/EncodedMessage.hpp` existed; every server target that reached both folders took the network one, and the build failed on `unknown type name 'EncodedMessage'`.

## Rule

1. **Before creating a header, look for its file name in the whole repository**: `git ls-files -- '*/<Name>.hpp'` must print nothing. Every target exposes its folder (`fix-architecture-one-cmakelists-per-folder.md`, point 4) and every include is written `"<Name>.hpp"`, so a target that reaches two folders holding the same name takes the first one, whatever the file meant.
2. **When the name is taken, choose another one, or rename the existing header first**, in a commit of its own before the commit that adds the new one, so each commit builds alone (`fix-execution-a-split-commit-carries-its-own-links.md`).
3. **A plan writes, next to each new header of its tree, that the name is free**, with the command of point 1 it ran.

Calibration: on `origin/main` at `edf3388`, `git ls-files -- '*/EncodedMessage.hpp'` prints `src/server/Replication/EncodedMessage.hpp`.

## Example

- ❌ **Before (wrong)**: `src/network/Protocol/EncodedMessage.hpp` created in the first commit, the server's `EncodedMessage.hpp` removed only in the third → the server and its tests stop building from the first commit on.
- ✅ **After (right)**: a `refactor` commit renames the server's struct `EventMessage` in `EventMessage.hpp` first; `git ls-files -- '*/EncodedMessage.hpp'` prints nothing; then the network `EncodedMessage.hpp` is added.
