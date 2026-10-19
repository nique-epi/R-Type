---
description: A convention is derived from at least three representative instances, including the most recent — never from a single example
trigger: always_on
---

# RULE: A convention is derived from a REPRESENTATIVE sample (≥ 3, including the most recent) — never from a single example

## Rule

1. **Before codifying or applying a convention** (naming, folder layout, commit format, PR template, shape of an ECS system), **read at least three complete instances**, including the most recent, and check that they converge. If they diverge, the divergence *is* the information: find what distinguishes the cases before writing anything.
2. **An isolated instance that differs from the others is a variant to understand, never the norm.**
3. **When the user clearly has a format in mind** ("follow what's already in place"), **show them the chosen skeleton before applying it at scale**.
4. **Never write an unmeasured coverage claim.** "Derived from the history" requires having read the history; otherwise write what you actually read.
5. Neighbors: `fix-reasoning-never-assume-unread-file-content.md` covers "I did not read"; `fix-reasoning-delegated-invariant-must-be-computed.md` covers "I read but paraphrased wrong"; this one covers **"I read ONE case and generalized it"**.

### The neighbour folder is not the sample: the written rule and the repository count first

1. **Before giving a new file the layout of the file next to it** (its own folder, a target of its own, a name), read the written rule that governs that layout (`.claude/rules/architecture/`, the project map) and apply it. A neighbour that departs from the rule is a pre-existing inconsistency to report, never the model to copy.
2. **When no rule decides, count across the repository, not the folder next door.** A layout repeated by two neighbours can be the minority everywhere else: list every instance of the kind (for example every free-function header with a `.cpp`) and follow the majority, or ask.
3. **Write in the plan which rule or count placed each new file**, so the reader can check the placement against its source.

## Example

- ❌ **Before (wrong)**: one commit has a `feat(server): …` scope → I make a scope mandatory on every commit.
- ✅ **After (right)**: `git log --format=%s -20` → most have no scope → the scope stays optional, and the exception is understood as such.
- ❌ **Before (wrong)**: `SessionDatagram.{hpp,cpp}`, a free-function header, given a folder of its own (`Replication/SessionDatagram/`) because `EntitySpawnSender/` next to it has one, while `architecture-one-folder-per-concrete-class.md` point 3 keeps free-function headers in their parent folder, where the placement review counted 13 of the repository's 19 such headers.
- ✅ **After (right)**: `src/server/Replication/SessionDatagram.{hpp,cpp}`, its target declared by `Replication/CMakeLists.txt` like `rtype_game_spawning` in `src/game/Spawning/`; `EntitySpawnSender`, touched by the same change, moved the same way; `WireEntityTypeMapping/`, untouched, reported.
