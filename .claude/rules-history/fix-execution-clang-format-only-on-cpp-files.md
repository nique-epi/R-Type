---
description: A list handed to clang-format -i holds only C++ files; clang-format rewrites any other file as C++
trigger: always_on
---

# RULE: A list handed to `clang-format -i` holds only C++ files

## Context

On the waiting room branch, after the `/simplify` pass, every changed and new file of `src/` and `tests/` was to be formatted before rebuilding the isolated copy. Nothing was committed yet, so the new files were untracked.

## Mistake

The tracked files were filtered on `.cpp` and `.hpp`, but the untracked ones were piped as they came: `git ls-files -o --exclude-standard -- src tests | xargs clang-format -i`. The list held `src/client/Screens/LobbyScreen/CMakeLists.txt`. clang-format exited without error and rewrote it as C++: `${` and `CMAKE_CURRENT_SOURCE_DIR}` split on two lines, every following line indented as a continuation. The file was restored from the isolated copy, synced before the pass, and compared with the plan.

## Root cause

The filter was applied to one of the two lists and not the other, on the belief that clang-format refuses a file it does not know; it chooses the language from the extension and falls back to C++.

## Rule

1. Filter every list passed to `clang-format -i` or `--dry-run` on the C++ extensions first, with quoted pathspecs: `git ls-files -z -o --exclude-standard -- '*.cpp' '*.hpp' | xargs -0 clang-format -i`.
2. After a bulk format, read `git status --short`, and the diff of any file in it that is not C++.

## Example

- ❌ **Before (wrong)**: `git ls-files -o --exclude-standard -- src tests | xargs clang-format -i` → the new `CMakeLists.txt` rewritten as code.
- ✅ **After (right)**: `git ls-files -z -o --exclude-standard -- '*.cpp' '*.hpp' | xargs -0 clang-format -i`, then `git status --short`.
