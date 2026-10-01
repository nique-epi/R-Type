---
description: touch on a list taken from git status recreates deleted files — filter on existence, and do not touch sources to force a rebuild
trigger: always_on
---

# RULE: `touch` on a list of paths recreates what was deleted — filter on what exists

## Rule

1. **Never pass a list taken from `git status` to `touch` without filtering on existence**:
   ```bash
   git diff --name-only --diff-filter=ACMR -z | xargs -0 touch
   ```
2. **Forcing a rebuild is done without touching the working tree**: `cmake --build build --clean-first`, or a fresh build folder. A build proof never modifies the sources it measures.
3. **A green build proves nothing about a deletion**: an empty file compiles, and a deleted `.cpp` still listed in a `CMakeLists.txt` only shows up at reconfigure. The index (`git diff --cached --name-status`) is what says what is deleted.

## Example

- ❌ **Before (wrong)**: `git status --porcelain | awk '{print $NF}' | xargs touch` → the two deleted files come back empty, build still green.
- ✅ **After (right)**: `cmake --build build --clean-first`, then `git diff --cached --name-status` where the two expected `D`s are present.
