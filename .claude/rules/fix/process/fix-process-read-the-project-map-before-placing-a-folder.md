---
description: Before a plan creates a folder or a module, read docs/src/content/docs/project-layout.md and place it where the map already says
trigger: always_on
---

# RULE: Read the project map before placing a new folder — the map may already say where it goes

Why: a plan put the starfield in a new `src/client/Background/` while `project-layout.md` already listed "starfield" under `src/client/Rendering/`; the mismatch only surfaced while writing the documentation, after the code.

## Rule

1. **Before a plan creates a folder, a module or a library**, read `docs/src/content/docs/project-layout.md`: the "Where does a new class go?" table and the tree of the program or library concerned. Search it for the feature's own words (`grep -n -i '<feature word>' docs/src/content/docs/*.md`).
2. **If the map already names a place for it, the plan uses that place**, or states in its decisions that it departs from the map, why, and that `project-layout.md` changes in the same pull request.
3. **The plan's tree cites the map line it follows** ("`Rendering/`: sprites, starfield, HUD…", `project-layout.md:150`), so the user validates the placement with the map in front of them.

### A test double follows the map, not its neighbours

1. **A test class that stands in for a production type or records what reaches it** (a `Recording…`, a `Fake…`, a subclass of a production interface or abstract class) **goes in `tests/doubles/`**, as the map says, even when the test folder next to it already holds doubles. Those are a pre-existing inconsistency to report, never the convention to follow.
2. **A test helper that only builds values** (`KeyPress`, `PointerEvents`, `MenuPainting`) stays next to the tests that use it.
3. **Decide which of the two a new test file is before creating it**, and say it in the plan's tree.

## Example

- ❌ **Before (wrong)**: plan with `src/client/Background/`, approved and coded; `project-layout.md` said `Rendering/  sprites, starfield, HUD, effects, lagometer`.
- ✅ **After (right)**: `grep -n -i starfield docs/src/content/docs/*.md` → the map puts it in `Rendering/` → the plan writes `src/client/Rendering/Background/` and quotes the map line.
- ❌ **Before (wrong)**: `RecordingRowControl`, a `RowControl` subclass recording the keys that reach it, written in `tests/client/Widgets/` because `RecordingInterfaceCanvas` sits in `tests/client/Rendering/`.
- ✅ **After (right)**: `tests/doubles/RecordingRowControl.{hpp,cpp}`, next to `RecordingSettingsStorage`; the doubles left in `tests/client/` reported.
