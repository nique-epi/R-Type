# RULE: No type is declared inside a class — every struct, class or enum has its own header

## Context

Pull request #103, the visual effects of the client (RTY-122). Yannis Lorang reviewed it on 23 October 2026 and requested changes. Among them: "Ne mets pas de struct dans les classes" (do not put structs in classes), and, on `EffectSpawner.hpp`, "Pourquoi cette classe a autant de méthodes et de structs" (why does this class have so many methods and structs).

## Mistake

The grep of point 4, run on the 92 headers the branch added or changed, printed 21 nested types:
- 14 the branch wrote: `EffectSpawner::SheetEffect`, `DownedShip`, `Shooter` and `HitSpot`; `FlashBudget::Loan`; `GainAttribution::Figure`, `Kill` and `Gain`; `PixelEffects::Particles`, `Wave` and `ArrivalLines`; `RenderSystem::SpriteLook`; `TextWriter::Line`; `GameEventHistory::SeenEvent`;
- 1 it moved: `LoadedFont`, from `InterfaceCanvas` into `TextWriter`;
- 6 already there at the base `fb17f12`, in headers it changed: `ServerConnection::Departure`, `RenderSystem::DrawCall` (whose members it changed), `InterfaceCanvas::SmoothImage`, `Squadron::ShipPlacement`, `Animation::Playback` and `GameScreen::Parts`, to which it added a member.

A first count, written from memory of the plan, gave `DrawCall` to the branch: git showed `RenderSystem.hpp` as deleted and added again, and the base file had not been read. An inventory that parsed every file with libclang, and read the base of each, corrected it.

## Root cause

Two written rules allowed it, and one prescribed it:
- `architecture-one-folder-per-concrete-class.md`, point 4: a private helper class "gets that helper its own folder too, or nests it as a type inside the class";
- `fix-architecture-at-most-three-parameters.md`, point 2.2: "The dependencies a class is built from go in a struct nested in that class: `KeySlot::Wiring`, `LobbyScreen::Parts`".

Nesting was the shortest way to name a type without creating a file, and the repository had many examples of it, so nothing stopped it.

## Rule

See the rule file of the same name in `.claude/rules/architecture/`: no type in a class body, one header per type named after it, plain structs in the parent folder, helper types only the `.cpp` reads in its anonymous namespace, a grep check calibrated on `LobbyScreen.hpp`, and nested types of other classes reported before being moved.

The two rules above were changed in the same pass: point 4 of `architecture-one-folder-per-concrete-class.md` no longer allows nesting, and point 2.2 of `fix-architecture-at-most-three-parameters.md` names the struct `<Class>Parts` in a header of its own.

## Example

- ❌ **Before (wrong)**: `FlashBudget::Loan`, a struct declared in the public part of `FlashBudget`, used as `FlashBudget::Loan{.area = …}` by `EffectSpawner`.
- ✅ **After (right)**: `struct FlashLoan` in `src/client/Rendering/Effects/FlashLoan.hpp`, used as `FlashLoan{.area = …}`.
