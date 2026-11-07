---
description: "A component is added or removed only once the query that found its entity has returned: the registry throws inside a forEach"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A component is added or removed only once the query that found its entity has returned

Why: `ComponentRegistry` throws "Components cannot be added or removed while a query is visiting components" when `add`, `remove` or `destroy` runs inside a `forEach`; a game process that throws stops, and the supervisor reports it crashed.

## Rule

1. **Inside a `forEach` lambda, only read and write the components it visits.** To add or remove a component, the lambda notes the entity (`std::optional<engine::Entity>` for one, a member `std::vector<engine::Entity>` for several), and the `add` or `remove` runs after `forEach` returns, as `bossNamed()` then `shield()` does in `BossArrivalEffects.cpp`.
2. **Before writing an `add` or a `remove`, read the enclosing lines**: a call inside the braces of a lambda handed to `forEach` is inside the query, however short the lambda.
3. **The tests of every target the change touches run before a build is handed over**, even when the user asked to skip the full gates for a quick test: build and run the test executables that link the changed libraries (`ctest --test-dir build -N -V -R <suite>` gives the executable). A full gate can wait; a crash the existing tests catch cannot.

## Example

- ❌ **Before (wrong)**: `markSummoned()` called `components.add(body, game::Summoning{…})` inside the `forEach<NetworkIdentity, EntityType>` that found the body; the build was handed over without running `server_weapon_system_tests`, and the first brood of the boss stopped the game.
- ✅ **After (right)**: the lambda stores `summoned = body`, the `add` runs after the `forEach`; `VolleySystemFixture.*` passes 5/5, and 3 of them fail with the registry's exception on the old version.
