---
description: "In src/, every .hpp with a .cpp of the same name lives with it in a folder named after them, with its CMakeLists.txt: classes, free functions and exceptions alike"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: In `src/`, a `.hpp` and its `.cpp` live in a folder named after them

Why: a review asked "put the cpp/hpp pairs in sub-folders"; the folder rule exempted free-function headers, so a pair of free functions stayed flat next to the folders of the classes.

## Rule

1. **Every `.hpp` of `src/` that has a `.cpp` of the same name lives with it in a folder named after them**: a concrete class, an abstract class with a `.cpp`, free functions, free functions with the struct they take, an exception. The folder holds those two files and its `CMakeLists.txt`, which declares their library and exposes the folder (`fix-architecture-one-cmakelists-per-folder.md`):
   ```
   src/client/Rendering/
   ├── CMakeLists.txt     add_subdirectory(Easing)
   └── Easing/
       ├── CMakeLists.txt add_library(rtype_client_easing STATIC Easing.cpp)
       ├── Easing.hpp
       └── Easing.cpp
   ```
2. **Only a header without a `.cpp` of its name stays in the parent folder**: an interface, a plain struct or enum, a `…Constants.hpp`, a header-only template.
3. **`tests/` is not concerned**: a fixture, a helper or a double sits next to its test or in `tests/doubles/`, its `.cpp` listed in `tests/CMakeLists.txt` (`fix-format-a-test-file-holds-only-tests.md`), as `tests/server/Replication/GameEventBroadcasterFixture.{hpp,cpp}` does.
4. **The `#include` lines do not change**: the files keep their names and the new folder is an include folder of the library. What changes is the parent `CMakeLists.txt` (`add_subdirectory` instead of the `add_library` block) and every documentation page that names the old path (`fix-process-grep-every-page-for-what-the-feature-replaced.md`).
5. **Check every `.cpp` of `src/` the change adds, changes or moves, before the push**:
   ```bash
   git diff --name-only --diff-filter=ACMR origin/main...HEAD -- 'src/*.cpp' | while read -r file; do
     folder=${file:h:t}; name=${file:t:r}
     [ -f "${file%.cpp}.hpp" ] || continue
     [ "$name" = "$folder" ] || echo "$file"
   done
   ```
   It must print nothing. A `.cpp` without a `.hpp` of its name (`main.cpp`, a platform file such as `FineTimerResolutionWindows.cpp`) is skipped. Calibration: run from `fb17f12` to `275b065`, it prints the six flat pairs of that branch (`ShiftedView`, `Easing`, `GroundEffects`, `FragmentShader`, `EntitySprites`, `MissionBanner`).
6. **A flat pair the change touches or moves gets its folder in the same change**; a flat pair it does not touch is reported (`fix-process-rework-preexisting-inconsistencies.md`). A file whose only change is an `#include` line rewritten because the header it names was renamed is not touched by the change: it is reported, not reworked. Measured on `origin/main` at `56a7019`: 29 flat pairs (21 of free functions, 6 exceptions, `World`, `RowControl`).

## Example

- ❌ **Before (wrong)**: `src/client/Rendering/Easing.{hpp,cpp}` and `src/client/Rendering/Background/ShiftedView.{hpp,cpp}`, flat, their targets declared by the `CMakeLists.txt` of the parent folder, while `IntegerScale`, `PixelRounding` and `RandomRange` sit in folders of their own in the same `Background/`.
- ✅ **After (right)**: `Rendering/Easing/` and `Background/ShiftedView/`, each with its `CMakeLists.txt` declaring `rtype_client_easing` and `rtype_client_shifted_view`; the parents call `add_subdirectory`; the check of point 5 prints nothing.
