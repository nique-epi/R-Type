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

## Update (2026-10-17) — two inputs, one assertion at the end

### Context

Options menu of the client (RTY-106), red proofs on an isolated copy.

### Mistake

`TheTabKeysAreReadByTheirPlace` tapped the key at the place of Q (printing A on AZERTY), then the key printing Q (at the place of A), and asserted only that the last tab had been asked for once. The variant that reads the tab keys by the character they print stayed green: it ignored the first tap and opened the tab on the second, the same end state.

### Root cause

The test checked the sum of two inputs whose effects swap between the right and the wrong code, so both readings produce one opening of the same tab.

### Rule

See the core file, subsection "One input per test when the faulty code reaches the same end state".

### Example

- ❌ `tap(Key::A, Scan::Q); tap(Key::Q, Scan::A); EXPECT_EQ(opened(), std::vector{LAST_TAB});`.
- ✅ Two tests: `tap(Key::A, Scan::Q)` asks for the last tab; `tap(Key::Q, Scan::A)` asks for none. The variant turns both red.
