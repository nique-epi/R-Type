# RULE: A test added to an existing executable links what its code uses

## Context

The help of the waiting room shares the corners of the squadron box through `paintBoxCorners`, a new library of `src/client/Rendering/Interface/BoxCorners/`. Its tests went into `client_widget_piece_tests`, the executable of the small drawing pieces, which needed no matcher until then.

## Mistake

`BoxCornersTest.cpp` compares the recorded fills with `EXPECT_THAT(…, UnorderedElementsAre(…))`. The executable listed `rtype_client_box_corners` in its `LIBS` but not `GTest::gmock`, so the full test workflow stopped at its link with undefined `testing::internal::UnorderedElementsAreMatcherImplBase` symbols.

## Root cause

The executable was chosen for its subject (drawing pieces), and its `LIBS` were extended with the library under test only; what the test file itself used, the gmock matchers, was never compared with what the executable linked.

## Rule

Before adding a test file to an existing executable, list what it uses (gmock matchers, `src/` headers, test helpers), read the executable's `LIBS` and include folders, add what is missing, and build that executable alone first.

## Example

- ❌ **Before (wrong)**: `LIBS rtype_client_tab_strip rtype_client_key_cap rtype_client_check_box rtype_client_outline rtype_client_box_corners`.
- ✅ **After (right)**: the same list ending with `GTest::gmock`.
