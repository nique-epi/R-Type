---
description: "A concrete class lives alone in its own folder, with only its .hpp, .cpp and CMakeLists.txt"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A concrete class lives alone in its own folder

Why: the user asked for it; a folder per class makes each class one library target with visible dependencies.

## Rule

1. **Every concrete class** (one that is neither an interface nor an abstract class) **has its own folder**, named after the class: `Time/SystemClock/` holds `SystemClock.hpp`, `SystemClock.cpp` and the folder's `CMakeLists.txt`, and nothing else.
2. **The folder contains only the class's `.hpp` and `.cpp`** (plus its `CMakeLists.txt`, see `fix-architecture-one-cmakelists-per-folder.md`). No second class, no helper, no constants file next to it.
3. **Interfaces, abstract classes, constants headers and free-function headers are exempt**: they stay in the parent folder (`Time/IClock.hpp`, `Time/TimeConstants.hpp`), which declares the target that exposes them.
4. **A class that needs a private helper class gets that helper its own folder too**, or nests it as a type inside the class.
5. **The folder's target is a library named after the class** (`rtype_engine_system_clock`), linking what it needs. A library that guards its dependencies with `rtype_forbid_links` forbids the forbidden modules by name, never the whole `rtype_` prefix, so it can link its own sub-libraries.
6. **A pre-existing concrete class outside a folder is reported** when touched and moved under `fix-process-rework-preexisting-inconsistencies.md`.

## Example

- ❌ **Before (wrong)**: `src/engine/Time/SystemClock.hpp` and `SystemClock.cpp` next to `FixedTimestep.hpp`.
- ✅ **After (right)**: `src/engine/Time/SystemClock/{CMakeLists.txt,SystemClock.hpp,SystemClock.cpp}` and `src/engine/Time/FixedTimestep/{…}`; `src/engine/Time/IClock.hpp` stays in `Time/`.
