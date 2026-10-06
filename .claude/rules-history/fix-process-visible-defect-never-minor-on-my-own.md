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
