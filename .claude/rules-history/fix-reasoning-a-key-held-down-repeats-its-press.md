# History: A key held down repeats its press

## Context

The leaderboard screen changes tab on Q and E, read from `sf::Event::KeyPressed`. `KeyChip`, on the same screen, already documents and tests that a key held down acts once.

## Mistake

A review measured it: six `keyPress(E)` with no release asked the source four times. The real screen would flip between its tabs, click, and send a request on each return while the key is held, and show ASKING for as long as the server ignores the requests.

## Root cause

The tab code was copied from `Menu::keyPressed`, which flips the tabs of the options the same way without any network; that one is pre-existing and reported in the pull request. A key repeat never shows in a test that presses a key once.

# RULE: A key held down repeats its press — an action taken on a press acts once per hold

Why: SFML turns key repeat on by default and `src/` never turns it off, so the `KeyPressed` of a held key arrives again and again.

## Rule

1. **Before acting on `sf::Event::KeyPressed`, decide what a repeat of it does.** Typing and moving through a list may repeat. An action that changes a screen, a tab or a state, sends a datagram or plays a sound acts once per hold.
2. **Latch it until the release**: a flag set by the press and cleared by the release and by `FocusLost`, as `readyKeyHeld_` in `LobbyScreen`; or act on the release of a press the widget saw, as `KeyChip`.
3. **Clear the latch when something else starts taking the events** (a dialog, a pushed screen): the release will not reach it.
4. **Test a repeat**: the same press twice with no release in between, the second doing nothing; then a release and a press, which acts again.

## Example

- ❌ **Before (wrong)**: `tabAskedBy()` opens the neighbouring tab on each `KeyPressed` of Q or E → with E held, the leaderboard flips between SERVER and GLOBAL, plays a click, and sends a `LeaderboardRequest` on each return.
- ✅ **After (right)**: `tabKeyHeld_` lets the first press through until a release or `FocusLost`; `AHeldTabKeyChangesTabOnce` presses E twice and checks the source was asked nothing more.
