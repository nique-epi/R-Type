---
description: A screen a module registers is listed in the registration test of that module
trigger: always_on
---

# RULE: A screen a module registers joins the registration test of that module

Why: `openingScreen()` returned a new `HEADPHONES_SCREEN` the menus module registered, while `MENU_SCREENS` did not list it: deleting the registration left the suite green, and the client would open on nothing and close at once, with no error.

## Rule

1. **Every `registry.add(<SCREEN>, …)` a change adds goes with a line in the list of its module** in `tests/client/Screens/ScreenModulesTestConstants.hpp` (`MENU_SCREENS`, `LOBBY_SCREENS`, `GAME_SCREENS`), which `ScreenModulesTest.cpp` checks.
2. **Before the push, compare the two lists**: `git grep -n 'registry.add(' -- src/client/<module>` against the constants of the test. A screen registered and not listed is added, the ones already missing included (`fix-process-rework-preexisting-inconsistencies.md`).

## Example

- ❌ **Before (wrong)**: `registry.add(HEADPHONES_SCREEN, buildHeadphonesScreen)` added, `MENU_SCREENS` left at 14 entries, four of them already missing (`LEAVE_GAME_CONFIRMATION_SCREEN`, `NEW_GAME_SCREEN`, `SIGN_IN_NEEDED_SCREEN`, `SIGN_IN_SCREEN`).
- ✅ **After (right)**: `HEADPHONES_SCREEN` listed with the screen, and the four others in a `test` commit of their own: 19 entries, one per `registry.add` of the menus module.
