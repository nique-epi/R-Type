---
description: "The screen stack updates the screens a pushed screen hides: list what they still do before pushing, and enter with replaceAll a screen every exit of which replaces the stack"
trigger: always_on
---

# RULE: A screen pushed over others leaves them running — what they still do must be decided

Why: `ScreenStack::update()` updates every screen, hidden ones included, so a navigation or a sound a hidden screen still has due happens under the new screen.

## Rule

1. **Before pushing a screen over others, list what each screen below still does in its `update()`**: a navigation it asked for or will ask for (`replaceAll(ScreenId::End)` once a game ended), a sound, a datagram. Decide, for each, whether it may happen under the new screen.
2. **A screen every exit of which is `replaceAll()` is entered with `replaceAll()`**, so the screens below stop; what it needs from them (their last picture, a result) is taken before.
3. **A `replaceAll()` the frame loop asks for is asked after `screens_.update()`**, so a navigation a screen asked for in the same frame is applied before it, and never undoes it.

### What stops receiving events drops what it holds

1. **Only the screen on top receives events, and a dialog or a help a screen holds takes them too.** Before events stop reaching a widget that tracks input (a key held, a press of the pointer, the pointer over it), give it `sf::Event::FocusLost`, so no release is awaited and nothing stays lit.
2. **List every way the events stop reaching it**: a `push()`, a dialog that opens, a help that opens. Each one needs it, not only the first one thought of.
3. **`FocusLost` clears everything the widget knows of the input**, the pointer over it included: nothing else will tell it where the pointer went.
4. **Test the hold across it**: a key pressed, the events taken, the key released once they come back; nothing acts.

## Example

- ❌ **Before (wrong)**: `screens_.push(ScreenId::Lost)` over the game: after a `GameEnded`, a `Disconnection` of the server within `END_SCREEN_DELAY` (1 s) shows the NO SIGNAL card, then the game still asks `replaceAll(ScreenId::End)`, and the end screen replaces the card.
- ✅ **After (right)**: after `screens_.update()`, `Application::run()` takes `framePictureOf()`, then asks `replaceAll(ScreenId::Lost)`; the screens of the session stop there.
- ❌ **Before (wrong)**: the room gives itself `FocusLost` before pushing the leaderboard, but the leaderboard opens its filter without telling its chips → Escape held across F leaves BACK drawn pressed, and its later release closes the screen.
- ✅ **After (right)**: `openFilter()` gives each chip `FocusLost` first; `TheFilterDropsWhatTheChipsHeld` releases Escape after the filter closed and checks nothing was asked.
