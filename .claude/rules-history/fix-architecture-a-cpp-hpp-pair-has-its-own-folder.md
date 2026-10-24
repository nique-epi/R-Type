# RULE: In `src/`, a `.hpp` and its `.cpp` live in a folder named after them

## Context

Pull request #103, the visual effects of the client (RTY-122). Yannis Lorang's review of 23 October 2026 asked "Mets bien les couples cpp/hpp dans des sous-dossiers" (put the cpp/hpp pairs in sub-folders).

## Mistake

The check of point 5, run on the branch, printed six flat pairs:
- four the branch wrote: `Background/ShiftedView`, `Rendering/Easing`, `Effects/Ground/GroundEffects`, `Widgets/MissionBanner`;
- two it moved: `Shaders/FragmentShader`, `Sprites/EntitySprites`.

In the same branch, `Effects/MissionStart/` had its own folder, and `Background/` already held `IntegerScale`, `PixelRounding` and `RandomRange` in folders of their own: the branch was not even consistent with itself.

## Root cause

`architecture-one-folder-per-concrete-class.md`, point 3, made "free-function headers" exempt from the folder, without saying anything of their `.cpp`. `fix-reasoning-convention-from-a-single-sample.md` then gave `SessionDatagram.{hpp,cpp}`, kept flat, as the right placement. On `origin/main` the two shapes were close: 24 free-function pairs in a folder of their own (15 of them network messages), 21 flat. Each new pair followed the neighbour or the exemption.

## Rule

See the rule file of the same name in `.claude/rules/architecture/`: every pair of `src/` in a folder of its name with its `CMakeLists.txt`; only headers without `.cpp` in the parent folder; `tests/` not concerned; includes unchanged; a check calibrated on the six pairs of the branch; a touched pair moved, an untouched one reported.

Changed in the same pass: point 3 of `architecture-one-folder-per-concrete-class.md` (only header-only files are exempt), the `SessionDatagram` example of `fix-reasoning-convention-from-a-single-sample.md`, and the conventions of `docs/src/content/docs/project-layout.md`.

## Example

- ❌ **Before (wrong)**: `src/client/Widgets/MissionBanner.{hpp,cpp}`, flat, its library `rtype_client_mission_banner` declared by `Widgets/CMakeLists.txt`.
- ✅ **After (right)**: `src/client/Widgets/MissionBanner/{CMakeLists.txt,MissionBanner.hpp,MissionBanner.cpp}`, `Widgets/CMakeLists.txt` calling `add_subdirectory(MissionBanner)`.
