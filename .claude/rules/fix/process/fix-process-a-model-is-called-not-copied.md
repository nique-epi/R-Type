---
description: Code written on the model of an existing function calls the shared steps, it never copies them
trigger: always_on
---

# RULE: Code written "on the model of" another function shares its steps, it never copies them

## Rule

1. **When a plan says "as X does" or "on the model of X"** for steps of code (loading a shader, reading a file, a retry loop), the change extracts those steps into one function that both call. The second copy is where the duplication starts.
2. **Search the neighbouring folders before writing a helper**: the reuse dimension of `core/code-review.md` applies while writing, not only at review.
3. **The extracted function follows the placement rules**: a free-function header and its target, in a folder every caller may link (`fix/architecture/fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`). A caller that would have to link back keeps its own code, and the report says why.

## Example

- ❌ **Before (wrong)**: `SfmlDrawSurface::loadFlashShader()` written "on the model of `InterfaceCanvas::loadCrispText()`": the same fifteen lines with other names.
- ✅ **After (right)**: `loadFragmentShader(FragmentShaderDescription, logger)` in `src/client/Rendering/FragmentShader.{hpp,cpp}`, called by both; `GaussianBlur` keeps its own loading, since `Rendering/Background/` cannot link a target of `Rendering/`.
