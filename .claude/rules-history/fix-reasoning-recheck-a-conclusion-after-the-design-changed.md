# History: recheck a conclusion after the design changed

## Context
The first plan for the game sprites drew every sprite straight into the window: `SfmlDrawSurface` held a reference to the window from its construction. The user asked to rely on the scrolling background, and the plan was rewritten: sprites are painted on a `PixelCanvas`, and the surface now holds the canvas, owned by the same object as the surface.

## Mistake
Explaining the client screen stack pull request later, the answer said that moving the entity drawing into `GameScreen` would require adjusting "the way `SfmlDrawSurface` keeps its target", because `GameScreen::draw()` receives the window each call. That constraint came from the first plan. With the canvas, the surface never sees the window, so nothing needs adjusting there.

## Root cause
The conflict analysis was written once, on the first design, and repeated after the design changed without being checked against the code as built.

## Rule
See `.claude/rules/fix/reasoning/fix-reasoning-recheck-a-conclusion-after-the-design-changed.md`.

## Example
- ❌ **Before (wrong)**: "the surface keeps its target, it will have to be adjusted".
- ✅ **After (right)**: "the surface draws on the entity canvas, owned with it by the game screen: nothing to adjust; only `show()` being non-const remains".
