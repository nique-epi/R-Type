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

## Example

- ❌ **Before (wrong)**: `screens_.push(ScreenId::Lost)` over the game: after a `GameEnded`, a `Disconnection` of the server within `END_SCREEN_DELAY` (1 s) shows the NO SIGNAL card, then the game still asks `replaceAll(ScreenId::End)`, and the end screen replaces the card.
- ✅ **After (right)**: after `screens_.update()`, `Application::run()` takes `framePictureOf()`, then asks `replaceAll(ScreenId::Lost)`; the screens of the session stop there.
