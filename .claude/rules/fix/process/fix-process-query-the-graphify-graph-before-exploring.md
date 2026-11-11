---
description: Before exploring the code of a task, query the graphify graph when one exists, then read only the files it names
trigger: always_on
---

# RULE: Before exploring the code, query the graphify graph when one exists — then read only what it names

Why: the user asked for it, to save tokens; a task that starts by reading whole files to find where a concept lives pays for thousands of lines the graph already indexed.

## Rule

1. **Before the first exploration of a task** (where a concept lives, who reads a component, what a change touches), look for the graph: `graphify-out/graph.json` at the root of the main checkout. The folder is ignored by git, so a worktree has none; its path is the first line of `git worktree list`.
2. **When it exists, ask it first**, with a budget: `graphify query "<question>" --graph <main checkout>/graphify-out/graph.json --budget 2500`. It answers with the nodes of the concept, each with its file and its line. `graphify explain "<symbol>"` and `graphify path "<A>" "<B>"` take the same `--graph`.
3. **Read only the files and the ranges it names**, never a whole folder to find them. A long file is read by ranges around the lines the graph gave.
4. **The graph says where to look, never that the list is complete.** A list every item of which must be changed (every reader of a component, every construction of a struct) is taken from `git grep`, as `fix/reasoning/fix-reasoning-validate-the-instrument-before-reporting-a-count.md` requires. The graph is older than the branch: a file it names is checked on disk before it is cited.
5. **Never build or update the graph unasked**: a build reads every file of the repository. Without a graph (a teammate's machine, the CI), explore with `git grep` and ranged reads, and say in one line that a graph can be built with `/graphify`.

## Example

- ❌ **Before (wrong)**: to find what reads the collision box of an entity, the 582 lines of `EntitySheetReader.cpp` and three other whole files read first, 33 KB in one output, while the graph of the main checkout was one command away.
- ✅ **After (right)**: `graphify query "which code reads the CollisionBox of an entity" --graph <main checkout>/graphify-out/graph.json --budget 2500` names `CollisionSystem`, `LagCompensatedHitSystem`, `OwnMissiles` and `EntityTypeLook` with their lines; `git grep -n -w 'CollisionBox' -- src` then gives the complete list of readers, and only those ranges are read.
