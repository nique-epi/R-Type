---
description: A library whose header a public header includes is linked PUBLIC, and every target is built before a header change is called done
trigger: always_on
---

# RULE: A library whose header a public header includes is linked `PUBLIC` — and the whole build proves it

Why: `GameScreen.hpp` included `LevelTransitionScene.hpp` while `rtype_client_game_module` linked `rtype_client_level_transition` `PRIVATE`; `r-type_client` built, and every other target that includes `GameScreen.hpp` stopped on `'LevelTransitionScene.hpp' file not found`.

## Rule

1. **Before adding an `#include` to a `.hpp` of a target, find the target that exposes the included header** and check how the target of the `.hpp` links it: `PUBLIC` (or `INTERFACE`) when the header is included by a header, `PRIVATE` only when only `.cpp` files include it. A `PRIVATE` link gives the include folder to the target alone, never to the targets that include its headers.
2. **When a new member of a class holds a type of another library**, that library goes in the `PUBLIC` list: the member is in the header.
3. **A header change is checked with the whole build**, every target, `cmake --build build --parallel <n> -- -k`, never with the program alone: the targets that include the header (tests, other modules) are where a missing include folder shows.

## Example

- ❌ **Before (wrong)**: `LevelTransitionScene transition_;` added to `GameScreen.hpp`, `rtype_client_level_transition` added to the `PRIVATE` list of `rtype_client_game_module`, and only `r-type_client` built → the full build fails on the test targets.
- ✅ **After (right)**: `rtype_client_level_transition` in the `PUBLIC` list, and `cmake --build build -- -k` builds every target.
