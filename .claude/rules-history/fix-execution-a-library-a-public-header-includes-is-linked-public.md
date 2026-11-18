# RULE: A library whose header a public header includes is linked `PUBLIC` — and the whole build proves it

## Context

The transition between two parts of a level gave `GameScreen` a member `LevelTransitionScene transition_;`, so `GameScreen.hpp` included `LevelTransitionScene.hpp`, the header of the new library `rtype_client_level_transition`.

## Mistake

The library was added to the `PRIVATE` list of `rtype_client_game_module`, next to `rtype_client_mission_start`, whose header only `GameScreen.cpp` includes. The check built `r-type_client` and the test executables of the transition, which passed. The user then reported that the project did not compile: the full build stopped on `GameScreen.hpp:35:10: fatal error: 'LevelTransitionScene.hpp' file not found` in every target that includes `GameScreen.hpp`.

## Root cause

The link was copied from its neighbour in the list without asking who includes the header: a `PRIVATE` link gives the include folder to the library alone. The check built only the targets of the change, so no target that includes `GameScreen.hpp` from outside the module was compiled.

## Rule

1. Before adding an `#include` to a `.hpp` of a target, find the target that exposes the included header and check how the target of the `.hpp` links it: `PUBLIC` (or `INTERFACE`) when the header is included by a header, `PRIVATE` only when only `.cpp` files include it.
2. When a new member of a class holds a type of another library, that library goes in the `PUBLIC` list.
3. A header change is checked with the whole build, every target, `cmake --build build --parallel <n> -- -k`.

## Example

- ❌ `rtype_client_level_transition` in the `PRIVATE` list of `rtype_client_game_module`, only `r-type_client` built.
- ✅ In the `PUBLIC` list, and `cmake --build build -- -k` builds every target.
