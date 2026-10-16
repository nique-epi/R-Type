# RULE: A static library is linked through one path only — never listed when another listed library already pulls it in

## Context

Options menu of the client (RTY-106), first full build of the feature on macOS (AppleClang, Apple ld).

## Mistake

Four links warned `ignoring duplicate libraries`: `r-type_client` (I had added `rtype_client_platform` and `rtype_client_settings`, already reached through `rtype_client_application`), `client_settings_tests` (`rtype_client_platform`, reached through the assets of `rtype_client_settings_file`), `client_interface_tests` (`outline`, `check_box`, `key_cap`, `tab_strip`, reached through the menu), and `client_screen_tests` (`widget_text`, `window_size_selection`, reached through the options screen). The compile had no warning, so the gate would have looked clean without reading the link step. `r-type_client` also listed `rtype_game`, which `main.cpp` does not use, a duplicate that predates the feature.

## Root cause

I listed every library whose header a file includes, without checking what the libraries already listed pull in. CMake keeps the order of the listed items and appends the libraries their `PRIVATE` links need afterwards, so a listed leaf is repeated after its dependents.

## Rule

See the core file.

## Example

- ❌ `rtype_add_test(… LIBS rtype_client_menu_builder rtype_client_outline …)`.
- ✅ A separate test executable listing only the pieces, dependents first, and `grep 'ignoring duplicate'` on the build log printing nothing.
