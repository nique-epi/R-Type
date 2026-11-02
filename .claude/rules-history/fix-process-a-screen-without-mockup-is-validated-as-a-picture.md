# RULE: A screen the mockup does not draw is validated as a picture before it is coded

## Context

Connection screen of the client (RTY-82). The approved mockup of the title screen ("1c v4, Hangar de pilote") draws the title page and the quit dialog, not the connection screen. The plan described the connection screen in words and with an ASCII sketch: three text fields drawn as menu bars `LABEL: text_`, in the left column of the title page, then CONNECT and BACK as full-size bars. The user approved the plan, the screen was coded, tested, simplified and reviewed.

## Mistake

Once the user saw the screen running, two things were wrong: the text fields did not look like something one can type into, and the menu was expected near the middle of the screen, with smaller buttons. The look of the fields came from the bar `PILOT: ACE` of the mockup's five-entry variant, read as "a field is a bar". Its toast says `PILOT → saisie du pseudo`: in the mockup it is a button that opens the nickname entry, not a text field.

## Root cause

The plan reused what was already written (the bar of `Button`, the column of `MenuBuilder::Frame::Page`) and justified it by an element of the mockup whose behavior was not read. No picture of the screen was shown before the code, so the user could only judge the look after the whole step was built.

## Rule

1. Before coding a screen or a widget the mockup does not draw, show a picture of it in the real style, with options when the look is open; code only the chosen one.
2. Each control takes its look from a precedent that does the same thing; read what the precedent does before reusing its look.
3. Where the screen sits on the window is part of the picture.

## Example

- ❌ **Before (wrong)**: fields drawn as bars `ADDRESS: 192.168.1.20_` in the left column, from the mockup's `PILOT: ACE` button, approved as an ASCII sketch.
- ✅ **After (right)**: the precedent is read (a button), pictures of the options are drawn in the conversation, the user picks one, then it is coded.

## Update (2026-10-15) — a menu input rule carried onto a text field

### Context

Connection screen, option A coded: a centered form whose three text fields sit above CONNECT and BACK.

### Mistake

Decision 17 of the plan kept the menu rule "the pointer selects the enabled bar it moves over" for the text fields, even while the player types, because it was "the rule of every menu". Once the screen ran, the user said the selected field must change on a click, not when the pointer passes over it.

### Root cause

The rule about precedents covered the look of a new control, not its input behavior. A rule written for menus of buttons, where hovering only moves a highlight, was kept on a control whose selection decides where the keyboard types.

### Rule

See the core file, subsection "A new kind of control takes its behavior from its precedent too".

### Example

- ❌ Hovering PORT while typing in ADDRESS moves the typing to PORT.
- ✅ In a form, only a click (or a key) moves the selection.

## Update (28 October 2026) — A new screen takes the input rules of the screen it repeats

### Context

RTY-125, the NO SIGNAL screen (`LostScreen`): it skips its closing on Enter or a click, then Enter presses PRESS ENTER TO RECONNECT, like the end screen skips its opening then presses PLAY AGAIN.

### Mistake

The screen acted on every `KeyPressed` and on a click inside the call only. A review found that the system repeats the press of a held key, so holding Enter skipped the closing, then pressed RECONNECT with the repeat; and that a click during the closing did nothing, since the call was not drawn yet. `EndScreen` already filtered the repeat (`releasesHeldKey()` and the repeat check) and skipped on any click.

### Root cause

The plan copied the look and the timing of the end screen's call, and wrote the input handling from scratch, without reading the input rules of the screen it repeated.

### Rule

See the core file, subsection "A new screen takes the input rules of the screen it repeats".

### Example

- ❌ Holding Enter on the NO SIGNAL card reconnects by itself.
- ✅ `releasesHeldKey()` moved to `Screens/HeldKey/`, shared by both screens; a held Enter acts once.

