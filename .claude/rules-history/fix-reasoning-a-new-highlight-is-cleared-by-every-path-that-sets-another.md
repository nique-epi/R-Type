# RULE: A new highlight is cleared by every path that sets the highlight it excludes

## Context

The hangar pull request added a star to the server map. Hovering it shows its corners and a bubble, and hides the bubble of the chosen server.

## Mistake

`hangarShown_` was set in the `MouseMoved` branch only, where `chosenShown_` was set to its opposite. The arrows and Tab set `chosenShown_ = true` and left `hangarShown_` alone: after hovering the star and pressing an arrow, the map painted both bubbles. A /simplify review found it; no test covered it.

## Root cause

The new flag was placed next to the one existing assignment of the flag it excludes, without listing the other assignments of that flag.

## Rule

See the core rule of the same name.

## Example

- ❌ `paintHangarStar(*canvas_, hangarShown_)`, arrows untouched.
- ✅ `paintHangarStar(*canvas_, hangarShown_ && !chosenShown_)` and the test `AnArrowHidesTheBubbleOfTheHangar`.
