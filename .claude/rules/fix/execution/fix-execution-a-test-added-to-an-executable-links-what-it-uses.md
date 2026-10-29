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

## Example

- ❌ **Before (wrong)**: `client/Rendering/Interface/BoxCornersTest.cpp` added to `client_widget_piece_tests` with `rtype_client_box_corners` only → undefined gmock symbols at the link.
- ✅ **After (right)**: `GTest::gmock` added to its `LIBS` with the corners library, and the executable built alone before the workflow.
