---
description: "A test file added to an existing test executable gets the libraries its code uses in that executable's LIBS: a gmock matcher needs GTest::gmock"
trigger: always_on
---

# RULE: A test added to an existing executable links what its code uses

Why: `BoxCornersTest.cpp` used `UnorderedElementsAre` in `client_widget_piece_tests`, whose `LIBS` hold gtest only; the link failed with undefined `UnorderedElementsAreMatcherImplBase` symbols.

## Rule

1. **Before adding a test file to the `SOURCES` of an existing `rtype_add_test`**, list what the file uses beyond its own subject: a gmock matcher or `EXPECT_THAT` needs `GTest::gmock`; a header of `src/` needs the target that exposes it; a test helper needs its `.cpp` in `SOURCES` and its folder in the include folders.
2. **Read the `LIBS` and `target_include_directories` of that executable** and add what is missing, minding `fix-execution-a-static-library-reached-twice-on-the-link-line.md`.
3. **Build that executable alone right after**, before the full workflow: `cmake --build build --target <executable>`.

### A double added to SOURCES brings the doubles it holds

1. **Before adding a double's `.cpp` to the `SOURCES` of an executable, read its header and the headers of its members**: each type of `tests/doubles/` it holds or names with a `.cpp` of its own goes in the same `SOURCES` (`PluginHostModule` holds a `PluginHostState`, which holds a `RecordingJournal`, defined in `RecordingJournal.cpp`).
2. **Copy the list from an executable that already links the double**: `git grep -n '<Double>.cpp' -- tests` shows where it is linked and what is linked next to it (`tests/doubles/CMakeLists.txt` gives `engine_tests` both `PluginHostModule.cpp` and `RecordingJournal.cpp`).

### A fixture is built by every executable that lists it

1. **Before adding an include to a fixture `.cpp`, list the executables that compile it**: `git grep -n '<Fixture>.cpp' -- 'tests/*CMakeLists.txt'`. Each one needs the library of the new header in its `LIBS`.
2. **Build each of them alone**, not only the executable of the test being written.

## Example

- ❌ **Before (wrong)**: `client/Rendering/Interface/BoxCornersTest.cpp` added to `client_widget_piece_tests` with `rtype_client_box_corners` only → undefined gmock symbols at the link.
- ✅ **After (right)**: `GTest::gmock` added to its `LIBS` with the corners library, and the executable built alone before the workflow.
