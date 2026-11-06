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

## Update (2026-11-05) — a double added to SOURCES brings the doubles it holds

### Context

The tests of the game statistics plugin run in `server_module_tests` and reach the journal through `PluginHostModule`, a double of `tests/doubles/` that only `engine_tests` linked until then.

### Mistake

`PluginHostModule.cpp` was added to the `SOURCES` of `server_module_tests`, but not `RecordingJournal.cpp`: the link failed with undefined `RecordingJournal::RecordingJournal(LogLevel)` and `RecordingJournal::lines() const`, referenced through `PluginHostState`.

### Root cause

Point 1 asked for the `.cpp` of a test helper, and the helper itself was added; the members it holds, doubles with a `.cpp` of their own, were not followed. `tests/doubles/CMakeLists.txt` already listed both files side by side for `engine_tests`.

### Rule

Read the header of a double and of its members before adding it, add every double they hold, and copy the list from an executable that already links it.
