---
description: "The window repeats KeyPressed while a key is held: an action taken on a press latches until the release, so holding a key acts once"
trigger: always_on
---

# RULE: A key held down repeats its press — an action taken on a press acts once per hold

Why: SFML turns key repeat on by default and `src/` never turns it off, so the `KeyPressed` of a held key arrives again and again.

## Rule

1. **Before acting on `sf::Event::KeyPressed`, decide what a repeat of it does.** Typing and moving through a list may repeat. An action that changes a screen, a tab or a state, sends a datagram or plays a sound acts once per hold.
2. **Latch it until the release**: a flag set by the press and cleared by the release and by `FocusLost`, as `readyKeyHeld_` in `LobbyScreen`; or act on the release of a press the widget saw, as `KeyChip`.
3. **Clear the latch when something else starts taking the events** (a dialog, a pushed screen): the release will not reach it.
4. **Test a repeat**: the same press twice with no release in between, the second doing nothing; then a release and a press, which acts again.

### A screen that gives way on its own waits for the release of a key it saw pressed

1. **Before a screen asks for the next screen on a timer, list what a key whose press it saw, still held, does then**: the system goes on repeating its press, and the next screen takes each repeat for a new press (`IntroScreen` opens the title on any press during its black second).
2. **Acting on the release protects only a key released before the screen leaves.** While such a key is held, the screen does not give way: it waits until the release, or until `FocusLost` clears the key.
3. **Test it past the deadline**: a press held past the time of the screen asks nothing; the release then lets the next screen come.

## Example

- ❌ **Before (wrong)**: `tabAskedBy()` opens the neighbouring tab on each `KeyPressed` of Q or E → with E held, the leaderboard flips between SERVER and GLOBAL, plays a click, and sends a `LeaderboardRequest` on each return.
- ✅ **After (right)**: `tabKeyHeld_` lets the first press through until a release or `FocusLost`; `AHeldTabKeyChangesTabOnce` presses E twice and checks the source was asked nothing more.
- ❌ **Before (wrong)**: `HeadphonesScreen` left on the release of a key, and gave way at 3.6 s whatever was held; Enter pressed at 1 s and held went on repeating into the intro, which opened the title.
- ✅ **After (right)**: `update()` returns while `heldKey_` holds a key; `AKeyHeldPastItsTimeKeepsTheIntroWaiting` holds Enter past 3.6 s, sees nothing asked, then releases it and sees the intro asked.
