---
description: Code written on the model of an existing function calls the shared steps, it never copies them
trigger: always_on
---

# RULE: Code written "on the model of" another function shares its steps, it never copies them

## Rule

1. **When a plan says "as X does" or "on the model of X"** for steps of code (loading a shader, reading a file, a retry loop), the change extracts those steps into one function that both call. The second copy is where the duplication starts.
2. **Search the neighbouring folders before writing a helper**: the reuse dimension of `core/code-review.md` applies while writing, not only at review.
3. **The extracted function follows the placement rules**: a free-function header and its target, in a folder every caller may link (`fix/architecture/fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`). A caller that would have to link back keeps its own code, and the report says why.

### A comment that names the model is the symptom

1. **Before writing a comment such as "as YOU on the ink of the flight log" next to steps or values**, share them: the comment says the copy exists and will drift. The shared values go in one constants header, and the comment goes away.

### A helper written in a second file is searched for by what it computes

1. **Before writing a helper of geometry or formatting** (where a chip lies, how a date reads), search the change itself and the files it touches for the same computation, by its arithmetic rather than its name.
2. **The shared step goes into the function both already call**, as a parameter, so no third copy can start.

## Example

- ❌ **Before (wrong)**: `SfmlDrawSurface::loadFlashShader()` written "on the model of `InterfaceCanvas::loadCrispText()`": the same fifteen lines with other names.
- ✅ **After (right)**: `loadFragmentShader(FragmentShaderDescription, logger)` in `src/client/Rendering/FragmentShader.{hpp,cpp}`, called by both; `GaussianBlur` keeps its own loading, since `Rendering/Background/` cannot link a target of `Rendering/`.
- ❌ **Before (wrong)**: `chipLeftOf()` in `LobbyScreen.cpp` and `chipAreaEndingAt()` in `LeaderboardScreen.cpp`, written in the same pull request, each moving the result of `MenuBuilder::chipArea()` so it ends at a given x.
- ✅ **After (right)**: `MenuBuilder::chipArea(text, right)`; the lobby ends a chip `BUTTON_GAP` to the left of its neighbour, and the leaderboard only moves it to its own row.
