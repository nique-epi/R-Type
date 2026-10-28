# History: one CMakeLists.txt per folder

## Context
The client window loop was being added. The plan put the new `GameWindow.cpp` straight into `src/client/CMakeLists.txt`, next to `main.cpp`.

## Mistake
A new class was added to a parent target instead of getting its own folder and its own `CMakeLists.txt`.

## Root cause
No rule described how the build is split, so the shortest edit (one more source in the existing target) was chosen.

## Rule
See `.claude/rules/fix/architecture/fix-architecture-one-cmakelists-per-folder.md`. The user's instruction: make a `CMakeLists.txt` per folder, and let the main one call the sub-ones.

## Update (2026-10-28) — a module declares one library

The rule still holds for every folder that holds sources. What changed is which folders hold sources: a module folder declares one library listing the `.cpp` of all its classes, and only a sub-module has a `CMakeLists.txt` of its own (`.claude/rules/architecture/architecture-a-module-is-one-folder-and-one-target.md`).
