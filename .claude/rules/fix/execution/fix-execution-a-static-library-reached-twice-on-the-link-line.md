---
description: Do not list a static library a target already reaches through a PRIVATE link of another listed library; the macOS linker warns on the duplicate
trigger: always_on
---

# RULE: A static library is linked through one path only — never listed when another listed library already pulls it in

Why: a static library reached through a `PRIVATE` link of a listed library is added again after it on the link line; the macOS linker then prints `ignoring duplicate libraries`, a warning the zero-warning gate counts.

## Rule

1. **Before adding a library to `target_link_libraries` or to the `LIBS` of `rtype_add_test`**, check whether a library already listed reaches it (`grep -rn <target> src/**/CMakeLists.txt`). If it does, do not list it again: get its headers through that path, or make the link that carries them `PUBLIC` when the headers of the carrying library name its types.
2. **A test that needs the header of a low-level piece** (a box, a keycap) **goes in an executable built on that piece**, not in one built on the menu that already pulls the piece in privately.
3. **Read the link step of every target the change touches**: `grep 'ignoring duplicate' <build log>` must print nothing, and `tr ' ' '\n' < build/<…>/link.txt | grep '\.a$' | sort | uniq -d` must print nothing for a target that warned.

## Example

- ❌ **Before (wrong)**: `client_interface_tests` lists `rtype_client_menu_builder` and `rtype_client_outline`; `outline` is also reached through `check_box`, a `PRIVATE` link of `button` → `ignoring duplicate libraries: librtype_client_outline.a`.
- ✅ **After (right)**: the tests of the drawing pieces go in `client_widget_piece_tests`, which lists only the pieces, dependents first (`tab_strip`, `key_cap`, `check_box`, `outline`); the link step is clean.
