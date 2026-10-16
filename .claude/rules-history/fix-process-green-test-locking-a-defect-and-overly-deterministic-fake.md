# RULE: An assertion describes the INTENDED behavior, never the observed one — and a double tamer than the real dependency makes a whole class of bugs invisible

This rule came with the rules imported when R-Type started; its first incident belongs to another repository. This file starts with its first update on this project.

## Update (2026-10-13) — the expected value is not the constant under test

### Context

Menu widgets of the title screen. The mockup says a pressed bar drops its label by one pixel; the code reads `PRESSED_TEXT_DROP` (1).

### Mistake

`APressedButtonIsPaintedPressed` expected `atRest + BUTTON_TEXT_OFFSET + sf::Vector2i(0, PRESSED_TEXT_DROP)`. A red proof setting `PRESSED_TEXT_DROP` to 0, rebuilt correctly, left every test green: the pressed label no longer moved, which the mockup forbids.

### Root cause

"Assert equality with the source" was read as "the constant of the code", while the source of the value is the requirement. A test that recomputes its expectation from the code's constant checks the arithmetic, not the requirement.

### Rule

See the core file, subsection "The expected value is not the constant under test".

### Example

- ❌ `EXPECT_EQ(label->anchor, atRest + BUTTON_TEXT_OFFSET + sf::Vector2i(0, PRESSED_TEXT_DROP));`
- ✅ `constexpr sf::Vector2i ONE_PIXEL_LOWER(0, 1);` then `EXPECT_EQ(label->anchor, atRest + BUTTON_TEXT_OFFSET + ONE_PIXEL_LOWER);` — the red proof turns it red.
