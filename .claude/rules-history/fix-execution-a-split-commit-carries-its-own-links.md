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
