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

### A rename that drops a qualifier carries its `using` line and its include

1. **When an early commit's version of a file comes from a scripted rename of the older version**, list the renames that change a qualified name into a bare one (`GainAttribution::Figure` → `GainFigure`, `GameScreen::Parts` → `GameScreenParts`). The older file reached the type through its owner's `using` line and header; the bare name needs its own.
2. **For each file the script rewrites with such a name, add the `using` line and the `#include` of the type's new header** in the same pass, then run `clang-format` on the result (`--assume-filename` set to the file's path in the repository): the longer or shorter name changes line breaks and include order.
3. **Build every early commit with `make -k`** (`cmake --build <dir> -- -k`), so one failing file does not hide the others: a plain build stops at the first target that fails, and the files of the targets it never reached are not checked.

## Example

- ❌ **Before (wrong)**: `feat: add a text field widget` committed with `TextFieldTest.cpp` in `client_interface_tests`, whose `LIBS` reach `TextField.hpp` only through `rtype_client_button`, which links `rtype_client_text_field` from the next commit → `fatal error: 'TextField.hpp' file not found` on that commit alone.
- ✅ **After (right)**: the first commit adds `rtype_client_text_field` to the test's `LIBS`; the second commit, where `Button` links it, removes it again; each commit is built and tested alone before the push.
- ❌ **Before (wrong)**: the refactor commit's `GainAttributionTest.cpp` made by rewriting `GainAttribution::Figure` as `GainFigure` in the older test, which only had `using rtype::client::GainAttribution;` → `unknown type name 'GainFigure'` on that commit alone; `PixelEffectsTest.cpp` and `GameScreenTest.cpp` had the same gap, unseen because the build stopped first.
- ✅ **After (right)**: each rewritten file gets `using rtype::client::GainFigure;` and `#include "GainFigure.hpp"`, is formatted, and the commit is rebuilt with `-- -k`.
