# RULE: A commit cut out of finished work carries the links its own files need

## Context

Connection screen of the client: the whole feature was written uncommitted, at the user's request, then split into six commits by intent before the push, two shared files staged as intermediate versions.

## Mistake

The first commit, `feat: add a text field widget`, added `TextFieldTest.cpp` to `client_interface_tests` without a link to `rtype_client_text_field`. In the finished tree the test reaches `TextField.hpp` through `rtype_client_menu_builder` → `rtype_client_button` → `rtype_client_text_field`, but the `Button` link only comes with the second commit. Built alone on the isolated copy, the first commit failed with `fatal error: 'TextField.hpp' file not found`. The check ran before the push, so nothing was published.

## Root cause

The split was decided by files and intents, and its staging checked only that the index matched the intent; the include paths a test target gets through its links were taken from the finished tree.

## Rule

1. Before staging an early commit of a split, check that each header its targets include is reachable through the links of that commit.
2. A link only an early commit needs is removed by the commit that makes it redundant.
3. Build and test every commit alone before the push.

## Example

- ❌ `TextFieldTest.cpp` in the first commit, `rtype_client_text_field` reached only through the second.
- ✅ The first commit links `rtype_client_text_field` to the test; the second removes that link when `Button` brings it.

## Update (2026-10-23) — A rename that drops a qualifier carries its `using` line and its include

### Context

The answer to a review of the visual effects was split into three commits (refactor, tests, docs), each to be built and tested alone before the push. The refactor commit renamed nested types (`GainAttribution::Figure` → `GainFigure`, `PixelEffects::Particles` → `PlacedBurst`, `GameScreen::Parts` → `GameScreenParts`), so the tests in that commit had to name the new types while keeping their old shape: a script produced them from the older versions by rewriting the names.

### Mistake

The script replaced the qualified names with the bare ones and added nothing else. `GainAttributionTest.cpp` had only `using rtype::client::GainAttribution;`, so the refactor commit failed alone with `unknown type name 'GainFigure'` (12 errors). `PixelEffectsTest.cpp` (`PlacedBurst`) and `GameScreenTest.cpp` (`GameScreenParts`) had the same gap, but the build stopped at the first failing target, so they were found by reading the script's output, not by the build. The rewrite had also left lines that `clang-format` would join. The check ran before the branch moved, so nothing was committed on the branch or pushed.

### Root cause

The script treated a rename as a text substitution. A qualified name is reached through its owner's declarations; once it is bare, it needs a `using` declaration and its own header, and the new length moves the line breaks.

### Rule

1. List the renames that make a qualified name bare.
2. Add the `using` line and the include of the new header to each file the script rewrites with one, then format the file.
3. Build each early commit with `-- -k`, so one failing file does not hide the others.
