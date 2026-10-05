# History: one CMakeLists.txt per folder

## Context
The client window loop was being added. The plan put the new `GameWindow.cpp` straight into `src/client/CMakeLists.txt`, next to `main.cpp`.

## Mistake
A new class was added to a parent target instead of getting its own folder and its own `CMakeLists.txt`.

## Root cause
No rule described how the build is split, so the shortest edit (one more source in the existing target) was chosen.

## Rule
See `.claude/rules/fix/architecture/fix-architecture-one-cmakelists-per-folder.md`. The user's instruction: make a `CMakeLists.txt` per folder, and let the main one call the sub-ones.
