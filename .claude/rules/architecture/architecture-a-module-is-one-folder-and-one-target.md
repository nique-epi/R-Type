---
description: "A module is one folder and one library target holding the .hpp/.cpp pairs of its classes flat, at most 10 .cpp; a sub-module is a sub-folder with a target of its own, only for a real sub-group"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A module is one folder and one target — its classes sit flat inside, a sub-module is the exception

Why: the user asked for it; a folder and a `CMakeLists.txt` per class made 14 of the 70 files of the waves pull request pure wiring, and every new class cost three files before it held a line.

## Rule

1. **A module is a folder of `src/` that groups the classes of one feature** (`src/server/Spawning/`, `src/game/Waves/`). It has one `CMakeLists.txt` declaring one library named `rtype_<owner>_<module in snake_case>`, which lists the `.cpp` of every class of the folder and exposes the folder (`fix-architecture-one-cmakelists-per-folder.md`). The word names a folder and a target: it is not `IModule`, and a module folder may hold a class implementing `IModule` or none.
2. **Inside a module, everything is a file of the module folder, never a folder of its own**: a concrete class, an abstract class, free functions, an exception, an interface, a plain struct, a constants header. `Spawning/WaveCatalog.hpp`, `Spawning/WaveCatalog.cpp`, `Spawning/WaveFileConstants.hpp`. One class per `.hpp`/`.cpp` pair, named after it (`code-style-file-named-after-its-class.md`); a helper class a class needs is a pair of the same module, never a type nested in it (`fix-architecture-no-type-nested-in-a-class.md`).
3. **A module holds at most 10 `.cpp` files**, the per-platform files of one class (`BorderlessScreenLinux.cpp`, `…MacOs.cpp`, `…Windows.cpp`, or a `Windows.cpp` and a `Posix.cpp`) counting as one. A module the change adds or flattens stays at 10 or fewer; one that would pass 10 is split into sub-modules (point 4) or into sibling modules. Measured on `origin/feat/server-modules` at `3ad9929`, counting the `.cpp` two levels under each folder: of 57 folders, 8 hold more than 8 class folders, 6 more than 10 and 4 more than 12 (`src/client/Widgets` 19, `src/network/Protocol/Messages` 15, `src/client/Rendering/Effects` 13, and `src/client` 20, a count that also takes in flat files). Those are not reworked by this rule alone (point 7).
4. **A sub-module is a sub-folder of a module with a `CMakeLists.txt` and a library of its own**, created only when a group of the module's classes meets one of:
   - the module would pass 10 `.cpp` and the group has a reason to change of its own;
   - the group needs a dependency the rest of the module must not link (a file parser, a platform API);
   - another module reuses the group.

   Never one per class, and never deeper than two levels: a sub-module has no sub-module. The parent links the sub-module and never the other way round (`fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`).
5. **Inside one module, the build no longer stops a class from including the class that includes it.** Two classes of a module never include each other, directly or through a third: when both need the same thing, it moves to an interface or a plain struct they both include.
6. **Tests mirror the module folder** (`tests/server/Spawning/`), one test file per class, as before; a test double stays in `tests/doubles/`.
7. **An existing layout with a folder per class stays valid until its module is flattened.** A folder holds either class folders or flat pairs, never both. A change that adds a class to a module still made of class folders either gives it a folder like its siblings, or flattens the module first in its own `refactor` commit (includes unchanged, targets merged into the module's, `CMakeLists.txt` and docs updated, `fix-process-grep-every-page-for-what-the-feature-replaced.md`); the pull request says which. A module the change does not touch is reported, not reworked.
8. **Check every module the change adds or changes, before the push**:
   ```bash
   git ls-files -- 'src/*.cpp' | sed -E 's#(Linux|MacOs|Windows|Posix)\.cpp$#.cpp#' | sort -u | sed -E 's#/[^/]*$##' | sort | uniq -c | awk '$1 > 10'
   ```
   It must print nothing for the folders the change touches. Calibration: with `> 3` instead of `> 10` it prints `src/client/Platform` (4 classes, 12 files before the per-platform names are merged); with `> 10` it prints nothing today, because every module is still made of class folders.

## Example

- ❌ **Before (wrong)**: two classes of `Spawning/` as `Spawning/WaveCatalog/{CMakeLists.txt,WaveCatalog.hpp,WaveCatalog.cpp}` and `Spawning/BydoSpawner/{CMakeLists.txt,BydoSpawner.hpp,BydoSpawner.cpp}`: six files, three of them `CMakeLists.txt`.
- ✅ **After (right)**:
  ```
  src/server/Spawning/
  ├── CMakeLists.txt        add_library(rtype_server_spawning STATIC BydoSpawner.cpp WaveDirector.cpp)
  ├── BydoSpawner.hpp
  ├── BydoSpawner.cpp
  ├── WaveDirector.hpp
  ├── WaveDirector.cpp
  └── WaveFile/             sub-module: the class that reads the waves file
      ├── CMakeLists.txt    add_library(rtype_server_wave_file STATIC WaveCatalog.cpp)
      ├── WaveCatalog.hpp
      ├── WaveCatalog.cpp
      └── WaveFileConstants.hpp
  ```
  `Spawning` links `rtype_server_wave_file`; `WaveFile/` links nothing from `Spawning/`.
