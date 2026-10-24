# RULE: A class declares at most ten member functions — a class that would pass it is split

## Context

Pull request #103, the visual effects of the client (RTY-122). Yannis Lorang's review of 23 October 2026 asked "Fais attention à la longueur des classes (nombre de méthodes)" and, on `EffectSpawner.hpp`, "Pourquoi cette classe a autant de méthodes et de structs". He gave no number; the user chose 10 on 23 October 2026, among 10 and 15.

## Mistake

Measured with libclang on the branch, member functions besides constructors and destructor:
- `EffectSpawner` 33: 6 at the base, 29 added by the branch, with 4 nested structs;
- the test scene `EffectScene` 23 (9 at the base);
- `EntityReplicatorFixture` 13 and `EntityReplicator` 13 (7 at the base each), pushed over 10 by the branch;
- `GameScreen` 12 (10 at the base): the branch added `flashPreference` and `moveSky`.
Over 10 before the branch and not given a member by it: `ServerConnection` 36, `Application` 14, `InterfaceCanvas` 14.

## Root cause

No rule bounded the size of a class. `fix-architecture-a-function-does-one-kind-of-step.md` split long functions, which multiplied the private functions of one class instead of moving them out: each split of `EffectSpawner` added two or three members to it. The same class gathered every effect because it was the only one `EntityReplicator` and `GameScreen` called.

## Sources read

- PMD, Java design rules, `TooManyMethods`: "A class with too many methods is probably a good suspect for refactoring, in order to reduce its complexity"; default `maxmethods` 10; getters, setters and `is…` methods of one statement are not counted (https://pmd.github.io/pmd/pmd_rules_java_design.html).
- SonarSource rule S1448 "Classes should not have too many methods", default 35 in its definition on the SonarSource tracker (https://sonarsource.atlassian.net/browse/RSPEC-1448); the current C++ page was not read.
- Distribution on `origin/main` at `56a7019`, 205 classes with member functions: median 1 member function besides constructors and destructor, 90th percentile 9, maximum 31.

## Rule

See the rule file of the same name in `.claude/rules/architecture/`: at most 10 member functions besides constructors, destructor and defaulted or deleted members; a class the change gives a member ends at 10 or fewer; a split along the state the functions touch; a `clang-query` count calibrated on `EffectSpawner`, `TextWriter` and `ServerConnection`; classes over the limit and untouched reported.

## Example

- ❌ **Before (wrong)**: `EffectSpawner`, 33 member functions, the only class the replicator and the game screen call for any effect.
- ✅ **After (right)**: `EffectSpawner` holds the classes the measured groups became and plays the ladder through them, each class at 10 member functions or fewer.
