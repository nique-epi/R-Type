# History: fix-reasoning-convention-from-a-single-sample

The rule was imported when the project started; its original incident belongs to another repository. This file starts with its first recurrence here.

## Update (19 October 2026) — The neighbour folder is not the sample

### Context

The server was taught to send every datagram to a connected player under one session header (token and next sequence). The plan added the free functions `nextSessionHeader`, `sessionHeaderFor` and `sessionDatagram` in a new header with its `.cpp`.

### Mistake

The plan put them in a folder of their own, `src/server/Replication/SessionDatagram/`, with its own `CMakeLists.txt`, because `EntitySpawnSender/` and `WireEntityTypeMapping/`, two free-function headers of the same folder, have one. The plan was approved and coded that way. An independent placement review then found that `architecture-one-folder-per-concrete-class.md` point 3 keeps free-function headers in their parent folder, which declares their target, and, with a script of its own, counted 13 of the 19 free-function headers with a `.cpp` in their parent folder (`src/game/Spawning/BydoSpawnPosition.hpp`, `src/network/Transport/EndpointText.hpp`…), against 6 in a folder of their own.

### Root cause

The layout was taken from the two neighbours of the same folder, read as the convention, without reading the written rule that decides it, nor counting the instances of the repository.

### Rule

1. Before giving a new file the layout of the file next to it, read the written rule that governs that layout and apply it; a neighbour that departs from it is a pre-existing inconsistency to report.
2. When no rule decides, count across the repository, not the folder next door.
3. The plan says which rule or count placed each new file.

### Example

- ❌ `src/server/Replication/SessionDatagram/{SessionDatagram.hpp,SessionDatagram.cpp,CMakeLists.txt}`, copied from `EntitySpawnSender/`.
- ✅ `src/server/Replication/SessionDatagram.{hpp,cpp}`, target declared in `src/server/Replication/CMakeLists.txt`; `EntitySpawnSender`, touched by the change, moved the same way; `WireEntityTypeMapping/` reported.

## Update (23 October 2026) — the placement of the example changed with the folder rule

Yannis Lorang's review of pull request #103 asked for every cpp/hpp pair in a folder of its own. Point 3 of `architecture-one-folder-per-concrete-class.md`, which the `SessionDatagram` example applied, now exempts header-only files only, and `fix-architecture-a-cpp-hpp-pair-has-its-own-folder.md` gives a free-function pair a folder. The example keeps its placement, marked as the rule then in force, with the placement the new rule gives; its lesson (read the written rule before copying a neighbour) is unchanged.
