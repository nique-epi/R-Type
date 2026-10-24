# RULE: A constant lives in a `…Constants.hpp`, and a `…Constants.hpp` holds only constants

## Context

Pull request #103, the visual effects of the client (RTY-122). Yannis Lorang's review of 23 October 2026 asked: "Renomme les fichiers ayant des constantes avec le préfixe constant" (rename the files holding constants with the "constant" prefix), and, inline, "renomme le fichier constant" on `src/client/Rendering/Effects/EffectColors.hpp` and on `EffectTiers.hpp`.

"Prefix" was read as the suffix `Constants`: no file of the repository starts with `Constants`, and all 78 `*Constants.hpp` on `origin/main` end with it, Yannis's four included (`PatternConstants.hpp`, `HealthConstants.hpp`, `BydoSpawnerConstants.hpp`, `GameEventTestConstants.hpp`).

## Mistake

Measured on the 92 headers the branch added or changed (an inventory of the pull request, each file read):
- 9 hold constants without being named `…Constants`: `EffectColors`, `EffectTiers` and `GhostShader`, written by the branch; `BackgroundColors`, `SpriteToneColors` and the test header `EffectLengths`, changed by it; `InterfaceColors`, `CrispTextShader` and `FlashShader`, moved by it.
- 5 headers of constants also define a type or a function: `GroundEffectConstants.hpp` (`Burst`, `Ring`), `EffectTiers.hpp` (`KillTier`), `EffectConstants.hpp` (`EffectAnimations`), `SquadronConstants.hpp` (`SquadronShip`), `InterfaceColors.hpp` (`PlayerColors`); `EntitySpriteConstants.hpp` gained the `constexpr` function `isPilotColor`.

On `origin/main`, 14 headers of constants are not named so (13 added by Loann Bideau, 1 by Nolan): the habit of naming a header after what it holds (`…Colors`, `…Shader`, `…Lengths`) came from the client.

## Root cause

`code-style-file-named-after-its-class.md` only gave `NetworkConstants.hpp` as an example of a file without class, and `fix-architecture-magic-values-as-named-constants.md` asked for "a dedicated constants header of the module, named after the concept". Neither said the name must end in `Constants`, nor that such a header holds nothing else, so a colors file or a shader source took the name of its concept alone, and a struct used by the constants went next to them.

## Rule

See the rule file of the same name in `.claude/rules/code-style/`: the `Constants` suffix for every header of constants, colors and shader sources included; constants only in it, the types and functions in their own headers; no constant elsewhere; two greps calibrated on `EffectColors.hpp` and `GroundEffectConstants.hpp`; a touched header renamed with its includes and documentation.

## Example

- ❌ **Before (wrong)**: `src/client/Rendering/Effects/EffectColors.hpp`, twenty colors and ramps, included by `EffectSpawner.cpp`.
- ✅ **After (right)**: `src/client/Rendering/Effects/EffectColorConstants.hpp`, the include of `EffectSpawner.cpp` and the two paths of `client.md` updated in the same commit.
