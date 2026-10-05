---
description: Build — one CMakeLists.txt per folder, the parent calls its children with add_subdirectory
trigger: always_on
---

# RULE: One `CMakeLists.txt` per folder, the parent adds its children with `add_subdirectory`

Why: the user asked for it; a single file listing every source of a module hides which folder owns which target.

## Rule

1. **Every folder holding sources has its own `CMakeLists.txt`** that declares the target of that folder (a `STATIC` library, or the executable for the entry-point folder).
2. **The parent `CMakeLists.txt` calls its children** with `add_subdirectory(<Folder>)` and links their targets. The root `CMakeLists.txt` calls `src/`, which calls `server/` and `client/`, which call their own subfolders.
3. **A parent never lists the `.cpp` files of a child folder** in its own `add_executable` / `add_library`.
4. **Include directories travel with the target**: the child exposes its folder with `target_include_directories(<target> PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})`, so callers include `"GameWindow.hpp"` without a relative path.
5. Every new target calls `rtype_enable_warnings()` (see `fix-execution-zero-warnings-proven-on-a-recompiling-build.md`).

## Example

- ❌ **Before (wrong)**: `add_executable(r-type_client main.cpp Window/GameWindow.cpp)` in `src/client/CMakeLists.txt`.
- ✅ **After (right)**: `src/client/Window/CMakeLists.txt` declares `rtype_client_window`; `src/client/CMakeLists.txt` does `add_subdirectory(Window)` and links `rtype_client_window` into `r-type_client`.
