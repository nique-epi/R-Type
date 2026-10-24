---
description: "Every named constant lives in a header named <Concept>Constants.hpp that holds constants only: colors, shader sources and test values included"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A constant lives in a `…Constants.hpp`, and a `…Constants.hpp` holds only constants

Why: a review asked to rename `EffectColors.hpp` and `EffectTiers.hpp`, two headers of constants whose name did not say so; the repository's other constants headers all end in `Constants` (`PatternConstants.hpp`, `HealthConstants.hpp`, `BydoSpawnerConstants.hpp`).

## Rule

1. **A header that defines a named constant is named `<Concept>Constants.hpp`**, `Constants` as a suffix, whatever the constants are: numbers, durations, colors, tables, a GLSL source, a shader description. A colors header is `EffectColorConstants.hpp`, not `EffectColors.hpp`; a shader source is `GhostShaderConstants.hpp`, not `GhostShader.hpp`.
2. **The constants of tests follow the same rule**, named `<Subject>TestConstants.hpp`; see `fix-format-a-test-file-holds-only-tests.md`.
3. **A `…Constants.hpp` holds constants only**: no struct, class, enum or alias definition, no function, `constexpr` functions and templates included. The type of a constant has its own header, named after it (`code-style-file-named-after-its-class.md`): `Burst.hpp` for the `Burst` that `KILL_DROPS` is made of, `KillTier.hpp` for `KILL_TIER`. A function goes in a free-function pair (`fix-architecture-a-cpp-hpp-pair-has-its-own-folder.md`), or in a header named after it when it must stay `constexpr`.
4. **No constant outside a `…Constants.hpp`**: not in a class header, not at the top of a `.cpp`, not in a test `.cpp` (`fix-architecture-magic-values-as-named-constants.md`, point 2).
5. **Check every header and `.cpp` the change adds or changes, before the push**, after `clang-format`:
   ```bash
   # a named constant outside a …Constants.hpp: must print nothing
   git grep -n -P '^(inline )?(constexpr|const) [^=;]*\b[A-Z][A-Z0-9_]+\b *(=|\(|\{)' -- <changed .hpp and .cpp files> | grep -v -E 'Constants\.hpp:'
   # a type or a function inside a …Constants.hpp: must print nothing
   git grep -n -P '^(struct|class|enum( class)?|union|using|template)\b|^(inline |constexpr |\[\[nodiscard\]\] )*[A-Za-z_:<>]+ [a-z][A-Za-z0-9]*\(' -- <changed *Constants.hpp files>
   ```
   - A hit inside a raw string literal (the GLSL source of a shader, its `void main()`) is not C++: read such hits, never count them.
   - The first line matches a definition whose name is in `UPPER_SNAKE_CASE`, the form of every constant here (`code-style-constants-without-k-prefix.md`), so a `constexpr` function in `camelCase` is not taken for a constant.
   - Both use `-P`: `git grep -E` (git 2.53) does not know `\b` and prints nothing, whatever the file holds.
   - Calibration, with `git grep` itself and a commit in place of `--`: `git grep -c -P '<first pattern>' 275b065 -- src/client/Rendering/Effects/EffectColors.hpp` prints 20; the second pattern at `275b065` prints `struct Burst` and `struct Ring` of `Ground/GroundEffectConstants.hpp` and `isPilotColor` of `Sprites/EntitySpriteConstants.hpp`.
6. **A header the change touches, moves or renames is renamed when it breaks points 1 or 3** (`code-style-file-named-after-its-class.md`: "renamed as soon as it is touched"), its includes and the documentation pages that name its path updated in the same change. A header the change does not touch is reported. A file whose only change is an `#include` line rewritten because the header it names was renamed is not touched by the change: it is reported, not reworked.

## Example

- ❌ **Before (wrong)**: `Effects/EffectColors.hpp` (twenty colors and ramps), `Effects/EffectTiers.hpp` (`struct KillTier` and the bursts, rings and tiers made of it), `Ground/GroundEffectConstants.hpp` defining `struct Burst` and `struct Ring` among its values.
- ✅ **After (right)**: `EffectColorConstants.hpp`, `EffectTierConstants.hpp` holding the values only, `KillTier.hpp`, `Burst.hpp` and `Ring.hpp` holding one struct each; both greps of point 5 print nothing.
