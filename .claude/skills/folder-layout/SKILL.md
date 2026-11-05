---
name: folder-layout
description: Arranges an R-Type folder into one sub-folder per category, and places what a change adds (a class, a folder, a test, its test constants, its fixtures, a test double) with the CMake wiring each needs. Use it whenever a refactor tidies a folder (a review says there are too many files side by side or asks for sub-folders, in src/ or tests/tools/), and whenever a change creates a file or a folder and must decide where it goes, even if the request only says "ranger", "sous-dossier", "trop de fichiers", "où va ce test", "où mettre la fixture" or "split this folder". Do not use it for a change made inside a file that already exists.
---

# Arranging a folder and placing its tests

A folder that holds every file side by side makes a reader open a list of names to find one class. This skill splits a folder into one sub-folder per category, and puts each new class, test, test constants header, fixture and double where the next reader will look first. It applies the rules; it does not replace them:

- `.claude/rules/architecture/architecture-a-module-is-one-folder-and-one-target.md`, points 6 to 18: what a category is, when a sub-folder exists, how deep, what stays at the root;
- `.claude/rules/code-style/fix-format-a-test-file-holds-only-tests.md`: what a test file holds, and where its constants and fixtures go;
- `.claude/rules/fix/process/fix-process-read-the-project-map-before-placing-a-folder.md`: the map first, doubles in `tests/doubles/`, tests mirror their source;
- `.claude/rules/fix/architecture/`: one `CMakeLists.txt` per folder, header names unique in the repository, no link pointing back.

## When to use it, and when not

| The change… | This skill |
|---|---|
| is a refactor whose purpose is to arrange a folder | applies to the whole folder |
| adds a file or a folder: a class, a module, a test, a `…TestConstants.hpp`, a fixture, a double | applies to the new files only |
| edits a file that already exists: a fix, a member function, a test case added to an existing test file | **does not apply**: the file stays where it is, its folder is neither split nor moved |

A new class added to a folder goes into the sub-folder of its category when the folder already has sub-folders. In a flat folder it goes flat, next to the others; if it makes a second category of two classes, say so in the report and propose the split as a refactor of its own. Existing files never move as a side effect of another change.

A folder seen out of layout while doing something else is named in one line of the report, never reworked.

## 1. Read the folder

1. Read the map, `docs/src/content/docs/project-layout.md`: the tree of the program or tool, and whether it already names a place for what you add (`grep -n -i '<word>' docs/src/content/docs/*.md`).
2. List the types of the folder from its headers, never from its `.cpp` files: a plain struct, an enum and a constants header are types too.
   ```bash
   git ls-files -- '<folder>/*.hpp' | grep -v -E '(Test|Fixtures?|TestConstants)\.hpp$'
   ```
3. List who includes whom inside the folder, to see which types travel together:
   ```bash
   grep -n '#include "' <folder>/*.hpp <folder>/*.cpp
   ```
4. Count the tests of each test file before anything moves; this is the number the check of step 6 compares with:
   ```bash
   for file in <folder>/*Test.cpp; do printf '%s ' "$file"; grep -c -E '^TEST(_F|_P)?\(' "$file"; done
   ```

## 2. Group the types into categories, and write the table

1. **A category is a role a reader names in a few words, holding at least two types**: "the pilot" (what a bot presses), "the tally" (what a bot counts). The helper types of a class count with it: its constants header, a plain struct only it builds or reads.
2. **A folder with two categories or more has one sub-folder per category.** A folder with a single category stays flat, however many files it holds.
3. **The root keeps what serves the whole folder**: the module class, the constants header every category reads, the interfaces several categories use. When nothing is left at the root, the root keeps only its `CMakeLists.txt`.
4. **A class two categories need goes into the one that needs it first**; the other includes it from there. Includes between two sibling sub-folders go one way only.
5. **Depth**: levels count from the folder of the program or the tool (`src/client/`, `tests/tools/LoadTestBots/`), three at most. A sub-folder splits again only when it holds two categories of its own. A `Fixtures/` folder never counts as a level.
6. **Write the table before the first file moves**, in the plan or the pull request:

   | Folder | Category | Types | Result |
   |---|---|---|---|
   | `tests/tools/LoadTestBots/Play/` | the pilot | `BotPilot`, `BotPilotConstants`, `BotInputSender` | `Play/Pilot/` |
   | | the tally | `BotTally`, `BotReport` | `Play/Tally/` |

## 3. Place the tests

| File | For a class of `src/` | For a class of `tests/tools/` |
|---|---|---|
| `<Class>Test.cpp` | `tests/<the path of the class>/`, every level of the path | next to the class, in its category folder |
| `<Class>TestConstants.hpp` | next to the test | next to the test |
| `<Class>Fixture.{hpp,cpp}` (the `TEST_F` class) | next to the test | in the `Fixtures/` folder beside the test |
| `<Class>Fixtures.{hpp,cpp}` (helper functions) | next to the test | in the `Fixtures/` folder beside the test |
| a test double (`Recording…`, `Fake…`, a subclass of a production interface) | `tests/doubles/` | `tests/doubles/` |
| a constant two test files need | the shared test header of their common folder | the same |

A test file holds only its includes, its `using` lines and its tests (`fix-format-a-test-file-holds-only-tests.md`). Before writing a helper or a constant, search the shared test headers for it (`git grep -n -w '<name>' -- 'tests/*.hpp'`).

## 4. Wire CMake

Every include folder is flat: a file includes `"BotPilot.hpp"`, never a path. Moving a file therefore changes no `#include`, as long as the target of its new folder is reached by whoever includes it. A new header takes a name no header of the repository has: `git ls-files -- '*/<Name>.hpp'` prints nothing.

**A category sub-folder** declares its library, named after its parent's plus its own name, and, in `tests/tools/`, the test executable of its tests:

```cmake
add_library(rtype_tools_load_test_bots_play_pilot STATIC
  BotInputSender.cpp
  BotPilot.cpp
)

target_include_directories(rtype_tools_load_test_bots_play_pilot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_load_test_bots_play_pilot PUBLIC rtype_client_connection_module)

rtype_enable_warnings(rtype_tools_load_test_bots_play_pilot)

rtype_add_test(NAME load_test_bots_play_pilot_tests SOURCES BotPilotTest.cpp BotInputSenderTest.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingPlayerInputLink.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RecordingWaitingRoom.cpp ${CMAKE_SOURCE_DIR}/tests/doubles/RefusingPlayerInputLink.cpp LIBS rtype_tools_load_test_bots_play_pilot)

target_include_directories(load_test_bots_play_pilot_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/doubles)

add_subdirectory(Fixtures)
```

**Its `Fixtures/` folder** declares no library: it hands its `.cpp` to the test executable of its parent, which calls `add_subdirectory(Fixtures)` after its `rtype_add_test`. A library of fixtures would have to link the library of its parent, the reverse of the allowed direction.

```cmake
target_sources(load_test_bots_play_pilot_tests PRIVATE
  BotInputSenderFixture.cpp
  BotPilotFixtures.cpp
)

target_include_directories(load_test_bots_play_pilot_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
```

**The parent** adds its sub-folders and keeps its old target name as an `INTERFACE` library that links them, so no caller changes, as `src/engine/Commands/` does:

```cmake
add_subdirectory(Pilot)
add_subdirectory(Tally)

add_library(rtype_tools_load_test_bots_play INTERFACE)

target_link_libraries(rtype_tools_load_test_bots_play
  INTERFACE
    rtype_tools_load_test_bots_play_pilot
    rtype_tools_load_test_bots_play_tally
)
```

**The tests of a folder of `src/`** are registered by the `CMakeLists.txt` of their own folder of `tests/`, which its parent adds with `add_subdirectory()`. That file declares the executable with `rtype_add_test()` when all its tests sit in that folder or below it; otherwise it hands its test files and fixtures to the executable a parent declares, as `tests/client/Rendering/Effects/Camera/CMakeLists.txt` does:

```cmake
target_sources(client_effect_tests PRIVATE CameraShakeTest.cpp CameraShakeFixtures.cpp)
```

Links go from a parent to its sub-folders, and one way between two siblings; never a link back at a folder that already links yours. A double goes in the `SOURCES` of the executable whose tests use it, and nowhere else.

## 5. Move

1. Create the folders, then `git mv` every file, so history follows it. A moved file is not edited.
2. Remove what the move empties (`git rm` an old `Fixtures/CMakeLists.txt`), then write the new `CMakeLists.txt` files.
3. A tidying refactor is one commit: `refactor: split <folder> into a folder per role`. New files go in the commit of the feature that adds them.

## 6. Check

1. Configure and build with `--parallel`, and read the log of the build that compiled the moved files: no `warning`, no `error`, no `ignoring duplicate libraries`.
2. Compare the test counts: `ctest --test-dir build -N` lists each executable; each new executable holds the sum of the counts of step 1 for its files, and the total did not change.
3. `cmake --workflow --preset test` green, `format-check` green, clang-tidy on every moved `.cpp` and `.hpp` (the CI lints renamed files).
4. Search every page and rule for the old paths and target names, and fix each hit:
   ```bash
   git grep -n -e '<old folder path>' -e '<old target name>' -- docs README.md .claude .github
   ```

## 7. Report

Give the table of categories, the resulting tree, the test counts before and after, the gates run with their results, the pages and rules fixed, and, in one line each, the folders seen out of layout and left as they are.

## Worked example: `tests/tools/LoadTestBots/Play/`

Before, 15 files side by side and a `Fixtures/` folder of 7, two roles mixed. After:

```
tests/tools/LoadTestBots/Play/
├── CMakeLists.txt                 add_subdirectory(Pilot) and (Tally), INTERFACE rtype_tools_load_test_bots_play
├── Pilot/                         what a bot presses, and when it sends it
│   ├── CMakeLists.txt             rtype_tools_load_test_bots_play_pilot, load_test_bots_play_pilot_tests
│   ├── BotPilot.{hpp,cpp}
│   ├── BotPilotConstants.hpp
│   ├── BotInputSender.{hpp,cpp}
│   ├── BotPilotTest.cpp
│   ├── BotPilotTestConstants.hpp
│   ├── BotInputSenderTest.cpp
│   ├── BotInputSenderTestConstants.hpp
│   └── Fixtures/
│       ├── CMakeLists.txt         target_sources(load_test_bots_play_pilot_tests …)
│       ├── BotPilotFixtures.{hpp,cpp}
│       └── BotInputSenderFixture.{hpp,cpp}
└── Tally/                         what a bot counts for the report
    ├── CMakeLists.txt             rtype_tools_load_test_bots_play_tally, load_test_bots_play_tally_tests
    ├── BotTally.{hpp,cpp}
    ├── BotReport.hpp
    ├── BotTallyTest.cpp
    ├── BotTallyTestConstants.hpp
    └── Fixtures/
        ├── CMakeLists.txt         target_sources(load_test_bots_play_tally_tests …)
        └── BotTallyFixtures.{hpp,cpp}
```

`BotInputSender` includes `BotPilot` and `BotTally` includes `BotReport`, so neither category includes the other. `Squad/` keeps linking `rtype_tools_load_test_bots_play`. The 23 tests of `load_test_bots_play_tests` became 12 in `load_test_bots_play_pilot_tests` (5 + 7) and 11 in `load_test_bots_play_tally_tests`.
