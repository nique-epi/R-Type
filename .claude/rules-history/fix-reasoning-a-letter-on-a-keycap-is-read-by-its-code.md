# RULE: A fixed key whose keycap shows a letter is read by its code — the scancode is a place, not a letter

## Context

The MAP bar of the waiting room votes on its key, M, whose keycap the bar shows. The other fixed keys of the room, L for RANKING and G for HANGAR, are `sf::Keyboard::Key` values compared on the `code` of an event, through `KeyChip`.

## Mistake

MAP_KEY was written `sf::Keyboard::Scan::M` and compared on the `scancode`, like the READY key, which is a binding. A scancode is the place of a key on a QWERTY keyboard; on the AZERTY keyboard of the user, the place of the QWERTY M holds the comma, and the M lies at the place of the QWERTY semicolon. The keycap said M and the M did nothing. The tests sent `Key::M` at `Scan::M`, so they passed.

## Root cause

The READY latch was copied for MAP without asking what each key is: READY is a binding, a place the player chose; MAP is a fixed letter the screen names.

## Rule

See `.claude/rules/fix/reasoning/fix-reasoning-a-letter-on-a-keycap-is-read-by-its-code.md`.
