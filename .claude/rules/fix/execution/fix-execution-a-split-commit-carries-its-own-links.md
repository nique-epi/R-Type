---
description: When finished work is split into several commits, each commit's targets must reach their headers through links of that same commit; build each commit alone before pushing
trigger: always_on
---

# RULE: A commit cut out of finished work carries the links its own files need

Why: in the finished tree a target often reaches a header through a link that a later commit adds; cut out alone, the earlier commit does not compile.

## Rule

1. **Before staging an early commit of a split**, list for each target it builds or changes (tests included) the headers its files include, and check that each one is reachable through the links of that commit, not of the finished tree. A test of a new library usually needs that library in its `LIBS` until a later commit links it through another target.
2. **A link only an early commit needs is removed by the commit that makes it redundant**, in the same commit, so the finished tree keeps one path (a static library listed twice makes the macOS linker warn).
3. **Build and test every commit alone before the push**, on an isolated copy, and read each build log before its test count: every `.cpp` the commit changes is recompiled, and the count grows by the tests the commit adds. Never push a split whose commits were not each built.

## Example

- ❌ **Before (wrong)**: `feat: add a text field widget` committed with `TextFieldTest.cpp` in `client_interface_tests`, whose `LIBS` reach `TextField.hpp` only through `rtype_client_button`, which links `rtype_client_text_field` from the next commit → `fatal error: 'TextField.hpp' file not found` on that commit alone.
- ✅ **After (right)**: the first commit adds `rtype_client_text_field` to the test's `LIBS`; the second commit, where `Button` links it, removes it again; each commit is built and tested alone before the push.
