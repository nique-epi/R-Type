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

### A new type takes a name no type of its namespace has

Why: the master's `rtype::network::LeaderboardRow`, in a header of its own, took the name of the struct `LeaderboardAnswer.hpp` already declares in the same namespace with other members; both programs link the two, which breaks the one-definition rule, and no build reports it, since each file sees one of them.

1. **Before declaring a struct, a class or an enum, search its name in the whole repository**, not only its file name: `git grep -n -P '^(struct|class|enum class|union) <Name>\b' -- src tests` must print nothing. A type that shares a header with others (a message and its row, a struct beside its function) is found only this way.
2. **The same holds for a free function at namespace scope** in a header: `git grep -n -P '\b<function>\(' -- 'src/*.hpp'`.
3. **When the name is taken in the same namespace, choose another one**, named after what sets the new type apart (`GlobalLeaderboardRow`, the row of the master's leaderboard).

Calibration: on `cdc75d8c`, `git grep -n -P '^(struct|class|enum class|union) LeaderboardRow\b' HEAD -- src tests` prints `LeaderboardAnswer.hpp:17`.

## Example

- ❌ **Before (wrong)**: `src/network/Protocol/EncodedMessage.hpp` created in the first commit, the server's `EncodedMessage.hpp` removed only in the third → the server and its tests stop building from the first commit on.
- ✅ **After (right)**: a `refactor` commit renames the server's struct `EventMessage` in `EventMessage.hpp` first; `git ls-files -- '*/EncodedMessage.hpp'` prints nothing; then the network `EncodedMessage.hpp` is added.
- ❌ **Before (wrong)**: `src/network/Master/Messages/LeaderboardRow.hpp` declared `rtype::network::LeaderboardRow`; `git ls-files` found no header of that name, and the struct of `LeaderboardAnswer.hpp` was never searched.
- ✅ **After (right)**: the type search of the subsection prints `LeaderboardAnswer.hpp:17`; the master's row is `GlobalLeaderboardRow`, in `GlobalLeaderboardRow.hpp`.
