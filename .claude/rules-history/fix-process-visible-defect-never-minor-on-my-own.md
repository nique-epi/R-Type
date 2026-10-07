# History: a visible defect is never ranked minor on my own authority

Rule imported from another project when R-Type started: its original incident belongs to another repository.

## Update (2026-10-06) — a missing resource is not a licence to downgrade what was asked

### Context
The user asked for buttons to pick the window size among five fixed sizes, as a provisional stand-in for the options menu.

### Mistake
The buttons were delivered as five unlabelled rectangles, the selected size being named in the window title instead, because SFML has no default font and the repository held none. The choice was explained after delivery. The user saw boxes with no text, two of which did nothing when clicked, before reading any explanation, and rejected them.

### Root cause
The missing font was treated as a constraint to design around rather than as something to obtain. A public-domain font was already on disk, in the sources of the SFML version the project builds, with its licence stated next to it; and an open issue covers asset loading. Neither was looked at before coding. The dimmed boxes for the sizes larger than the desktop carried no text either, so nothing on screen said why they did not react.

### What was done
The buttons now carry the size they give as text, with the font copied from the SFML sources into the repository, and a missing font raises an exception that names the file.

## Update (2026-10-06) — The residual defects are listed from the source, every one of them

### Context

Writing version 0 of the network protocol page (`docs/src/content/docs/protocol.md`). Version 0 guarantees no message; the page has a losses table saying, for each message, what corrects its loss. Several rows end with an effect that is simply missed: no explosion, no sound, no banner.

### Mistake

I presented the residual defect as "a lost `GameEvent`, `EntityDestroyed` or `PlayerLeft` loses a sound, an explosion or a banner" and offered to send those three messages three times. The user approved. The losses table I had written myself also said a lost `EntitySpawned` is corrected by the next state "without its appearance effect": the shot sound of a missile is lost the same way. It was not in the option. The omission surfaced only when the user asked how missiles travel and I traced the flow.

### Root cause

I listed the affected messages from memory of the ones discussed just before (the messages with no state to repeat them), not by walking the table row by row. `EntitySpawned` looked "corrected" because the entity itself comes back, while the effect it carries does not.

### Rule

Before presenting a residual defect and its fix, derive the affected items from the complete source, row by row; the option put to the user names every affected item; an omission found after the decision is said first and put to the user as its own choice.

### Example

- ❌ **Before (wrong)**: option (b) "send `GameEvent`, `EntityDestroyed`, `PlayerLeft` three times", approved, with `EntitySpawned` silently left out.
- ✅ **After (right)**: walk the losses table, list every message whose loss loses a one-time effect, `EntitySpawned` included, and put the complete list in the option.
