# History: A screen a module registers joins the registration test of that module

## Context

The client opens on `openingScreen()`, which asks `replaceAll()` of a screen some module registered; a screen nobody registered builds nothing, and the client closes with no error. `ScreenModulesTest.cpp` checks that each module registers the screens its list names.

## Mistake

The headphones notice made `openingScreen()` return `HEADPHONES_SCREEN`, registered by the menus module, without adding it to `MENU_SCREENS`. A review found that deleting the registration left every test green, and that four screens of the menus module were already missing from the list.

## Root cause

The list of the test is kept by hand, next to no code that registers the screens, and nothing compares the two.

Why: `openingScreen()` returned a new `HEADPHONES_SCREEN` the menus module registered, while `MENU_SCREENS` did not list it: deleting the registration left the suite green, and the client would open on nothing and close at once, with no error.

## Rule

1. **Every `registry.add(<SCREEN>, …)` a change adds goes with a line in the list of its module** in `tests/client/Screens/ScreenModulesTestConstants.hpp` (`MENU_SCREENS`, `LOBBY_SCREENS`, `GAME_SCREENS`), which `ScreenModulesTest.cpp` checks.
2. **Before the push, compare the two lists**: `git grep -n 'registry.add(' -- src/client/<module>` against the constants of the test. A screen registered and not listed is added, the ones already missing included (`fix-process-rework-preexisting-inconsistencies.md`).

## Example

- ❌ **Before (wrong)**: `registry.add(HEADPHONES_SCREEN, buildHeadphonesScreen)` added, `MENU_SCREENS` left at 14 entries, four of them already missing (`LEAVE_GAME_CONFIRMATION_SCREEN`, `NEW_GAME_SCREEN`, `SIGN_IN_NEEDED_SCREEN`, `SIGN_IN_SCREEN`).
- ✅ **After (right)**: `HEADPHONES_SCREEN` listed with the screen, and the four others in a `test` commit of their own: 19 entries, one per `registry.add` of the menus module.
