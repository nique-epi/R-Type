# RULE: Read the project map before placing a new folder — the map may already say where it goes

## Context

Scrolling starfield background of the client. The implementation plan created a new module `src/client/Background/` (about fifteen class folders), the user approved it, and the code was written and tested in that folder.

## Mistake

`docs/src/content/docs/project-layout.md`, the agreed map of the repository, already listed the starfield under the client's `Rendering/` folder ("`Rendering/` sprites, starfield, HUD, effects, lagometer"). The plan never read that page; the mismatch surfaced only when the `document-feature` skill opened it, after the code and its gates.

## Root cause

The structure was derived from the code tree alone (`git ls-files src`), while the repository's own `CLAUDE.md` says to read the repository before assuming a structure, and the map of planned folders lives in the documentation, not in the code.

## Rule

1. Before a plan creates a folder, a module or a library, read `project-layout.md` and search it for the feature's own words.
2. If the map names a place, use it, or state the departure, its reason and the map update in the plan's decisions.
3. The plan's tree cites the map line it follows.

## Example

- ❌ **Before (wrong)**: `src/client/Background/` planned and coded while the map said `Rendering/  sprites, starfield…`.
- ✅ **After (right)**: `grep -n -i starfield docs/src/content/docs/*.md` first, then `src/client/Rendering/Background/` with the map line quoted in the plan.

## Update (2026-10-17) — A test double follows the map, not its neighbours

### Context

Options menu of the client. The tests of `Menu` needed a row control that records the keys, presses and activations reaching it: `RecordingRowControl`, a subclass of the abstract `RowControl`. The same change added two other doubles, `FakeSizableWindow` and `RecordingSettingsStorage`, to `tests/doubles/`.

### Mistake

`RecordingRowControl` was written in `tests/client/Widgets/`, next to the menu tests, because the doubles of the interface tests already sat beside them (`RecordingInterfaceCanvas` in `tests/client/Rendering/`, `RecordingNavigator` in `tests/client/Screens/`). The map says test doubles go in `tests/doubles/`; the fresh placement review reported it, and the change was inconsistent with itself.

### Root cause

The neighbours were taken as the convention, while the map, the written rule, says otherwise; the existing doubles outside `tests/doubles/` are a pre-existing inconsistency.

### Rule (added)

1. **A test class that stands in for a production type or records what reaches it** (a `Recording…`, a `Fake…`, a subclass of a production interface or abstract class) **goes in `tests/doubles/`**, as the map says, even when the test folder next to it already holds doubles. Those are a pre-existing inconsistency to report, never the convention to follow.
2. **A test helper that only builds values** (`KeyPress`, `PointerEvents`, `MenuPainting`) stays next to the tests that use it.
3. **Decide which of the two a new test file is before creating it**, and say it in the plan's tree.

### Example

- ❌ **Before (wrong)**: `RecordingRowControl`, a `RowControl` subclass recording the keys that reach it, written in `tests/client/Widgets/` because `RecordingInterfaceCanvas` sits in `tests/client/Rendering/`.
- ✅ **After (right)**: `tests/doubles/RecordingRowControl.{hpp,cpp}`, next to `RecordingSettingsStorage`; the doubles left in `tests/client/` reported.
