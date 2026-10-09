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
