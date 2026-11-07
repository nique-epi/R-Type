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

## Update (2026-11-03) — A call after the input undid what the assertion read

### Context

`SessionAcknowledgements` keeps, on the client, the sequences of the session and the wait of an acknowledgement; `acknowledgeIn()` writes the acknowledgement into a header and ends the wait, and `close()` must forget both.

### Mistake

One test closed the session, let `ACKNOWLEDGEMENT_DELAY` pass, called `acknowledgeIn()`, then asserted that nothing was due and that the header acknowledged nothing. The red proof removed the reset of the wait from `close()` and the test stayed green: `acknowledgeIn()` had ended the wait before the assertion read it.

### Root cause

The test checked two facts after a sequence of calls without reading whether a later call reset the state of the first fact.

### Rule

Read every call between the input and the assertion for one that resets the state asserted; assert before it, or give each fact its own test.

### Example

- ❌ **Before (wrong)**: `close()`, `advance()`, `acknowledgeIn()`, then `EXPECT_FALSE(isAcknowledgementDue())`.
- ✅ **After (right)**: two tests, one per fact, each red on its own variant.

## Update (3 November 2026) — A timer walked in one step, and a color two states share

The tests of `ServerLeaderboard` jumped five seconds in one `update()`: removing either timer reset of the class left them green, while the client would have sent a request each frame. `ThePointerOverItLooksSelected` looked for `SELECTED_BAR_INSIDE_COLOR`, which is `EDGE_BLUE`, the border of a chip at rest too, so it could not fail; writing the opposite test, with the focus lost, showed it.

## Update (5 November 2026) — A job stopped before its thread reached its loop

`ServerStatusJobFixture.StoppingTheJobDoesNotWaitForItsInterval` built the job with an interval of an hour and stopped it at once. The thread had not started its loop yet: it saw the stop and left, so the variant that slept with `std::this_thread::sleep_for`, deaf to the stop, passed in 0.02 s. `TheJobRefreshesEveryInterval` registered its server right after building the job, so the first pass often saw it, and the variant that refreshed only once stayed green. Both tests now wait until a first server is online, the sign the first pass ran, before they stop the job or register the second server; each variant then turns its test red.

## Update (7 November 2026) — A guard backed up by the lookup after it

`AdminConsoleCommandsFixture.RefusesWhatIsNotANickname` ran `kick` with 17 bytes and asserted that the line was refused and nothing sent. Without `isValidNickname()` in `nicknameNamedBy()`, the 17 bytes went on to `gamesPlaying()`, which found no game holding such a pilot and refused the line too, so the red proof stayed green. The test now also asserts the reason, `'ABCDEFGHIJKLMNOPQ' is not a nickname`, which only the nickname check writes, and the variant turns it red.
