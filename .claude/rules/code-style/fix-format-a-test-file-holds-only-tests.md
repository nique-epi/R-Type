---
description: "A test .cpp holds only its tests: constants in <Subject>TestConstants.hpp, fixtures and helpers in their own .hpp/.cpp, no anonymous namespace"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A test file holds only its tests — its constants and its fixtures live in files of their own

Why: a review asked, for eleven places of one pull request, to move the constants of the test files into files and to move the fixtures out of them; the repository already does it in `WorldStateSenderTest.cpp`, `GameEventBroadcasterTest.cpp` and `UdpSocketTest.cpp`.

## Rule

1. **A `…Test.cpp` holds its `#include` lines, its `using` declarations, and its `TEST`, `TEST_F` and `TEST_P` blocks with their Given / When / Then comments. Nothing else**: no anonymous namespace, no constant, no struct, class or enum, no free function, no lambda at namespace scope.
2. **Its constants go in `<Subject>TestConstants.hpp`**, in the same folder as the test, `<Subject>` being the class or header under test: `GameEventHistoryTestConstants.hpp` for `GameEventHistoryTest.cpp`.
   - Search the shared test headers first (`fix-architecture-magic-values-as-named-constants.md`, "A test constant is searched for before it is defined"). A value two test files need goes in the shared header of their common folder, never in both.
   - The header holds constants only (`fix-format-a-constants-header-ends-in-constants.md`) and uses the namespace the test headers of its folder use: `rtype::<module>::testing` where they do, none where they do not.
   - Two constants of the same name and different values must never be reachable from one test: give each a name that says what it is (`WHERE_THE_BYDO_DIED`, not `WHERE_IT_DIED` twice).
3. **Its fixtures go in files of their own**, next to the test, each `.hpp` declaring and its `.cpp` defining (`architecture-hpp-declarations-cpp-definitions.md`; template bodies stay in the header):
   - a `TEST_F` class → `<Subject>Fixture.{hpp,cpp}`, the class named `<Subject>Fixture` and used as the suite name;
   - a scene the tests build (registries and systems, `GroundScene`) → a `TEST_F` fixture, or a class in `<ClassName>.{hpp,cpp}` as `tests/CollisionScene.{hpp,cpp}` does;
   - free helper functions and the structs they return (`shakeKeeping`, `ShakeOverASecond`) → `<Subject>Fixtures.{hpp,cpp}`, as `ProtocolFixtures.hpp` does;
   - a class that stands in for a production interface → `tests/doubles/` (`fix-process-read-the-project-map-before-placing-a-folder.md`).
   The constants a fixture uses are in the `…TestConstants.hpp`, never defined in the fixture files. Each fixture `.cpp` is listed in the `SOURCES` of its test target in `tests/CMakeLists.txt`.
4. **A test file the change adds or changes is brought to this shape whole**, its constants and fixtures already there included, in a `refactor` or `test` commit of its own (`fix-process-rework-preexisting-inconsistencies.md`). A test file the change only moves is reported. A file whose only change is an `#include` line rewritten because the header it names was renamed is not touched by the change: it is reported, not reworked.
5. **Check every `…Test.cpp` the change adds or changes, before the push**, after `clang-format`:
   ```bash
   git grep -n -E '^namespace \{|^(struct|class|union|enum( class)?) [A-Za-z_]|^(inline )?(constexpr|const) |^(template ?<[^>]*> ?)?(\[\[nodiscard\]\] )?(static )?[A-Za-z_][A-Za-z0-9_:<>, ]*[ *&]+[a-z][A-Za-z0-9_]*\([^;]*$' -- <changed *Test.cpp files>
   ```
   It must print nothing. Calibration: on `git show '275b065:tests/client/Rendering/Effects/Camera/CameraShakeTest.cpp'` it prints the anonymous namespace, eight constants, `struct ShakeOverASecond` and `shakeKeeping`; on `git show 'origin/main:tests/server/Replication/WorldStateSenderTest.cpp'` it prints nothing.

## Example

- ❌ **Before (wrong)**: `GroundEffectsTest.cpp` opens an anonymous namespace holding nine constants (`ON_PIXEL_100_50`, `YOUNG`, `RING_RADIUS`…), `class GroundScene` with its bodies, and `shrinkingDropAged()`, then its tests.
- ✅ **After (right)**: `GroundEffectsTestConstants.hpp` holds the nine constants; `GroundEffectsFixture.hpp` declares the fixture class `GroundEffectsFixture` (the former scene) and `GroundEffectsFixture.cpp` defines it; `GroundEffectsTest.cpp` holds its includes, its `using` lines and `TEST_F(GroundEffectsFixture, …)` blocks, and the grep of point 5 prints nothing.
