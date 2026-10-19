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
