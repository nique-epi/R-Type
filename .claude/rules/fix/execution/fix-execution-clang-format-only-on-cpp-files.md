---
description: A list handed to clang-format -i holds only C++ files; clang-format rewrites any other file as C++
trigger: always_on
---

# RULE: A list handed to `clang-format -i` holds only C++ files

Why: clang-format picks the language from the extension and falls back to C++ for an unknown one, so a `CMakeLists.txt` in its list is rewritten as code, with no error.

## Rule

1. **Filter every list passed to `clang-format -i` or `--dry-run` on the C++ extensions first**, with quoted pathspecs: `git ls-files -z -o --exclude-standard -- '*.cpp' '*.hpp' | xargs -0 clang-format -i`, or `grep -E '\.(cpp|hpp|tpp)$'` on a list made otherwise. A list of "the new files" or "the changed files" also holds `CMakeLists.txt`, Markdown and assets.
2. **After a bulk format, read `git status --short`**, and the diff of any file in it that is not C++: such a file in the list is the symptom.

## Example

- ❌ **Before (wrong)**: `git ls-files -o --exclude-standard -- src tests | xargs clang-format -i` → the new `src/client/Screens/LobbyScreen/CMakeLists.txt` rewritten with `${` and `CMAKE_CURRENT_SOURCE_DIR}` on two lines and every line indented as code.
- ✅ **After (right)**: `git ls-files -z -o --exclude-standard -- '*.cpp' '*.hpp' | xargs -0 clang-format -i`, then `git status --short`.
