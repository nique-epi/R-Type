# RULE: Code written "on the model of" another function shares its steps, it never copies them

## Context

The client animations draw a hit flash with a fragment shader. The approved plan said `SfmlDrawSurface` would load it "on the model of `InterfaceCanvas::loadCrispText()`".

## Mistake

`SfmlDrawSurface::loadFlashShader()` repeated `loadCrispText()` step by step: check that shaders are available and log a warning, compile from memory and log an error, set the texture uniform, keep the shader in an `std::optional`. The reuse review found the two copies (and a third, different one in `GaussianBlur`).

## Root cause

"On the model of" was read as a pattern to copy, not as a function to share.

## Rule

1. When a plan says "as X does" or "on the model of X" for steps of code, the change extracts those steps into one function that both call.
2. Search the neighbouring folders before writing a helper.
3. The extracted function follows the placement and link rules; a caller that would have to link back keeps its own code, and the report says why.

## Example

- ❌ **Before (wrong)**: `SfmlDrawSurface::loadFlashShader()`, the same fifteen lines as `InterfaceCanvas::loadCrispText()`.
- ✅ **After (right)**: `loadFragmentShader()` in `src/client/Rendering/FragmentShader.{hpp,cpp}`, called by both.

## Update (3 November 2026) — A comment naming the model

The YOU tag of `ItemList` copied the steps and the four values of the tag of the flight log, and its constants said so in a comment: "as YOU on the ink of the flight log". `paintInkTag()` now serves both.

## Update (3 November 2026) — Two helpers of one pull request

The leaderboard placed its chips with `chipAreaEndingAt()`, and the waiting room placed RANKING and HELP with `chipLeftOf()`; both took `MenuBuilder::chipArea()` and moved the chip so it ends at a given x. They were written in the same pull request, in two files, under two names, so neither search by name found the other; a review did. `MenuBuilder::chipArea()` now takes the right edge.
