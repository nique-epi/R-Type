---
description: A fixed key whose keycap shows a letter is compared by its code, which follows the layout, never by its scancode
trigger: always_on
---

# RULE: A fixed key whose keycap shows a letter is read by its code — the scancode is a place, not a letter

Why: MAP read `Scan::M`, the place of the M of a QWERTY keyboard; on an AZERTY keyboard that place holds the comma, and the M the keycap names never voted.

## Rule

1. **A key the screen fixes and names on a keycap by its letter** (M for MAP, L, G, H) **is compared on `code`** (`sf::Keyboard::Key`), which SFML gives from the layout: the player presses the letter it reads. `keyDownOf(event, sf::Keyboard::Key)` and `KeyChip` do it.
2. **A key the player binds is compared on `scancode`**: a binding names a place, and its keycap shows the name the layout gives that place (`localizedKeyName`).
3. **A test of a fixed letter sends the event of a layout where the place and the letter differ**: `keyPress(sf::Keyboard::Key::M, sf::Keyboard::Scan::Semicolon)`, the M of an AZERTY keyboard. A test that sends the QWERTY place passes on the faulty code.

## Example

- ❌ **Before (wrong)**: `constexpr sf::Keyboard::Scancode MAP_KEY = sf::Keyboard::Scan::M;`, tested with `keyPress(Key::M, Scan::M)` → green tests, no vote from the M of an AZERTY Mac.
- ✅ **After (right)**: `constexpr sf::Keyboard::Key MAP_KEY = sf::Keyboard::Key::M;` read with `keyDownOf(event, MAP_KEY)`, and the tests press `Key::M` at `Scan::Semicolon`.
