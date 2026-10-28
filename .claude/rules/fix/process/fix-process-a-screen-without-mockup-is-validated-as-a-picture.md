---
description: A screen the mockup does not draw is shown to the user as a picture, and chosen, before its code; each control takes its look and its input behavior from a precedent that behaves the same
trigger: always_on
---

# RULE: A screen the mockup does not draw is validated as a picture before it is coded

Why: a layout described in words or ASCII is approved without being seen; the user judges the screen only once it runs, after the code and its tests are written.

## Rule

1. **Before coding a screen or a widget the mockup does not draw**, show the user a picture of it in the real style: the pixel grid, the fonts, the colors and the sizes of the client, drawn in the conversation (an inline widget), with two or three options when the look is open. Code only the option the user picks. An ASCII sketch in a plan is not that picture.
2. **Each control takes its look from a precedent that does the same thing.** Before reusing the look of an element of the mockup, read what that element does (its action, its toast, its states): a button that opens a nickname entry is a button, not a text field. A control the player types into looks editable (an inset box, a caret, a label outside it); a control the player activates looks pressable. Never give a new kind of control the look of another kind because the widget is already written.
3. **Where a screen sits on the window is part of the picture**: the column of the title screen is a precedent for pages of the title, not for every screen. Ask, or show the options, when a screen could be a page, a centered panel or a dialog.

### A new kind of control takes its behavior from its precedent too

1. **An input rule shared by a menu (the pointer selects what it moves over, a click presses) is reread for each new kind of control**: does it still serve what the player does with that control? A control the player types into keeps the keyboard until the player moves it on purpose (a click, Tab, an arrow), as in any form: a pointer passing over another bar must not take the typing away.
2. **"It is the rule of every menu" is never the reason to keep a behavior on a new kind of control.** Write in the plan, for each shared input rule, whether it applies to the new control and why, so the user can see it before the code.

### A new screen takes the input rules of the screen it repeats

1. **Before writing the key and pointer handling of a new screen, read the `handleEvent()` of the screen whose behavior it repeats** (`EndScreen` for a screen that skips an opening, then acts on Enter) and list its input rules: a key held down acts once, a click anywhere skips the opening, which keys act and when.
2. **Keep each rule through the shared helper** (`releasesHeldKey()`, `src/client/Screens/HeldKey/`), never by leaving it out; a rule the new screen drops on purpose is written in its doc comment with the reason.
3. **Test the held key**: a second `KeyPressed` of the same key with no release in between acts on nothing.

### Example (behavior)

- ❌ **Before (wrong)**: the plan kept "the pointer selects the bar it moves over" for the text fields of the connection screen, "the rule of every menu"; on screen, the field being typed in changed as soon as the pointer moved.
- ✅ **After (right)**: a form selects a bar on a click only; pages and dialogs, made of buttons, keep the selection under the pointer.
- ❌ **Before (wrong)**: `LostScreen` acted on every `KeyPressed`; the system repeats the press of a held key, so holding Enter skipped the closing, then pressed RECONNECT by itself.
- ✅ **After (right)**: `LostScreen` keeps the key pressed last, as `EndScreen` does, and forgets it on `releasesHeldKey()`; `AHeldEnterActsOnce` checks it.

## Example

- ❌ **Before (wrong)**: the connection screen drew its three text fields as menu bars reading `ADDRESS: 192.168.1.20_`, because the mockup had a bar `PILOT: ACE`, and laid them in the left column of the title page; the plan showed an ASCII sketch. On screen, the fields did not look editable and the menu was not where the user expected it.
- ✅ **After (right)**: the mockup's `PILOT: ACE` is read first (its toast opens the nickname entry: a button); two pictures of the screen are drawn in the conversation, an inset box with a label above and a terminal line, in a centered panel with smaller buttons; the user picks one, then it is coded.
