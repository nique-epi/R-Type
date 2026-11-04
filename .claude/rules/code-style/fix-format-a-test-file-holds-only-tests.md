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
   - The header holds constants only (`fix-format-a-constants-header-ends-in-constants.md`) and uses the namespace the test headers of its folder use: `rtype::<owner>::testing` where they do, none where they do not.
   - Two constants of the same name and different values must never be reachable from one test: give each a name that says what it is (`WHERE_THE_BYDO_DIED`, not `WHERE_IT_DIED` twice).
3. **Its fixtures go in files of their own**, next to the test (in `tests/tools/`, in the `Fixtures/` folder beside it: see the subsection below), each `.hpp` declaring and its `.cpp` defining (`architecture-hpp-declarations-cpp-definitions.md`; template bodies stay in the header):
   - a `TEST_F` class → `<Subject>Fixture.{hpp,cpp}`, the class named `<Subject>Fixture` and used as the suite name;
   - a scene the tests build (registries and systems, `GroundScene`) → a `TEST_F` fixture, or a class in `<ClassName>.{hpp,cpp}` as `tests/CollisionScene.{hpp,cpp}` does;
   - free helper functions and the structs they return (`shakeKeeping`, `ShakeOverASecond`) → `<Subject>Fixtures.{hpp,cpp}`, as `ProtocolFixtures.hpp` does;
   - a class that stands in for a production interface → `tests/doubles/` (`fix-process-read-the-project-map-before-placing-a-folder.md`).
   The constants a fixture uses are in the `…TestConstants.hpp`, never defined in the fixture files. Each fixture `.cpp` is listed in the `SOURCES` of its test target in `tests/CMakeLists.txt`; in `tests/tools/`, the `CMakeLists.txt` of its `Fixtures/` folder adds it.
4. **A test file the change adds or changes is brought to this shape whole**, its constants and fixtures already there included, in a `refactor` or `test` commit of its own (`fix-process-rework-preexisting-inconsistencies.md`). A test file the change only moves is reported. A file whose only change is an `#include` line rewritten because the header it names was renamed is not touched by the change: it is reported, not reworked.
5. **Check every `…Test.cpp` the change adds or changes, before the push**, after `clang-format`:
   ```bash
   git grep -n -E '^namespace \{|^(struct|class|union|enum( class)?) [A-Za-z_]|^(inline )?(constexpr|const) |^(template ?<[^>]*> ?)?(\[\[nodiscard\]\] )?(static )?[A-Za-z_][A-Za-z0-9_:<>, ]*[ *&]+[a-z][A-Za-z0-9_]*\([^;]*$' -- <changed *Test.cpp files>
   ```
   It must print nothing. Calibration: on `git show '275b065:tests/client/Rendering/Effects/Camera/CameraShakeTest.cpp'` it prints the anonymous namespace, eight constants, `struct ShakeOverASecond` and `shakeKeeping`; on `git show 'origin/main:tests/server/Replication/Announcements/Streams/WorldStateSenderTest.cpp'` it prints nothing.

### A helper taken out of an anonymous namespace is searched for in its new namespace

1. **A free function of an anonymous namespace is local to its file; moved into `<Subject>Fixtures.{hpp,cpp}`, it becomes an external function of the named namespace** (`rtype::server::testing`), where a shared test helper may already define it. Two external definitions of one function break the one-definition rule; the link only fails the day a test pulls in the object of the other one.
2. **Before moving each helper, search the test headers for its name**, from the repository root:
   ```bash
   git grep -n -w -e '<helper>' -e '<other helper>' -- 'tests/*.hpp' | grep -E '\('
   ```
   For each hit, read its namespace and signature. Same namespace and signature: delete the moved copy and include that header. Same purpose under another name (`onlyMessageIn` for `messageOf`): call it instead. Another namespace or signature (`frameOf` of the client sprites): no collision.
3. **Calibration**, at commit `a90a9a0b`, with `-e playerEndpoint -e messageTypeOf -e messageOf`: the command prints `tests/server/SentDatagrams.hpp:21`, the `playerEndpoint` of the same namespace the moved copy collided with.

### In `tests/tools/`, the fixtures of a folder go in its `Fixtures/` sub-folder

1. **In a folder of `tests/tools/`, the fixture and helper files of point 3 go in a `Fixtures/` sub-folder of the folder that holds their tests**: `tests/tools/LoadTestBots/Play/Fixtures/BotPilotFixtures.{hpp,cpp}` for `tests/tools/LoadTestBots/Play/BotPilotTest.cpp`. The `…TestConstants.hpp` stay next to the tests (point 2). The rest of `tests/` keeps its fixtures next to their test.
2. **`Fixtures/` has a `CMakeLists.txt` of its own, which gives its `.cpp` to the test target of its parent and declares no library**:
   ```cmake
   target_sources(load_test_bots_play_tests PRIVATE BotPilotFixtures.cpp)
   target_include_directories(load_test_bots_play_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
   ```
   The parent calls `add_subdirectory(Fixtures)` after its `rtype_add_test`, and never lists a `.cpp` of `Fixtures/` itself. A library of fixtures would have to link the library of its parent, the reverse of the direction `architecture-a-module-is-one-folder-and-one-target.md` (point 9) allows.

## Example

- ❌ **Before (wrong)**: `GroundEffectsTest.cpp` opens an anonymous namespace holding nine constants (`ON_PIXEL_100_50`, `YOUNG`, `RING_RADIUS`…), `class GroundScene` with its bodies, and `shrinkingDropAged()`, then its tests.
- ✅ **After (right)**: `GroundEffectsTestConstants.hpp` holds the nine constants; `GroundEffectsFixture.hpp` declares the fixture class `GroundEffectsFixture` (the former scene) and `GroundEffectsFixture.cpp` defines it; `GroundEffectsTest.cpp` holds its includes, its `using` lines and `TEST_F(GroundEffectsFixture, …)` blocks, and the grep of point 5 prints nothing.
- ❌ **Before (wrong)**: `playerEndpoint`, `messageTypeOf` and `messageOf` moved from the anonymous namespace of `ConnectionAcceptorTest.cpp` into `ConnectionAcceptorFixtures.{hpp,cpp}`, in `rtype::server::testing`, where `SentDatagrams.hpp` already declares `playerEndpoint(std::size_t)` and offers `onlyMessageTypeOf` and `onlyMessageIn`.
- ✅ **After (right)**: the search of the subsection prints `SentDatagrams.hpp:21`; the copies are deleted, the tests call `playerEndpoint`, `onlyMessageTypeOf` and `onlyMessageIn` from `SentDatagrams.hpp`.
- ❌ **Before (wrong)**: `BotPilotFixtures.{hpp,cpp}` next to `BotPilotTest.cpp` in `tests/tools/LoadTestBots/Play/`, among the classes of the tool, listed in the `SOURCES` of `load_test_bots_play_tests` by `Play/CMakeLists.txt`.
- ✅ **After (right)**: in `Play/Fixtures/`, with `BotInputSenderFixture` and `BotTallyFixtures`, whose `CMakeLists.txt` adds the three `.cpp` to `load_test_bots_play_tests` with `target_sources`.
