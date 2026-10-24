# RULE: A test file holds only its tests — its constants and its fixtures live in files of their own

## Context

Pull request #103, the visual effects of the client (RTY-122). Yannis Lorang's review of 23 October 2026 asked "Déplace les fixtures dans des fichiers" and "Déplace les constantes des fichiers tests dans des fichiers", with inline comments on `GameEventHistoryTest.cpp:13`, `ShiftedViewTest.cpp:11`, `StarfieldTest.cpp:54`, `CameraShakeTest.cpp:15` and `:33`, and `GroundEffectsTest.cpp:34` and `:45`.

## Mistake

An inventory of the 43 test files the branch added, changed or moved (each file read) found:
- every test the branch wrote opened an anonymous namespace of constants; eight of them also defined a scene class or helper functions there (`GroundScene`, `PixelScene`, `GroundEffectScene`, `SpriteEffectScene`, `shakeKeeping`, `bigCatchesOf`, `knowingBothPilots`…);
- `EntityReplicatorTest.cpp` had no anonymous namespace at the base: the branch added one, while the same branch put `FLYING_PLAYER_LIVES` in `EntityReplicatorTestConstants.hpp`;
- duplicates across files: `EVENT_NUMBER` in `GameEventHistoryTest.cpp` and `ServerConnectionTestConstants.hpp`, `TOP_WARP_FACTOR` and `TOP_WARP_SPEED`, `CLOSE_KILLS`, `SCORE_BEFORE` and `DRIFTER_POINTS` in two tests each;
- same names with different values in tests of one folder (`WHERE_IT_DIED`, `RAMP`, `START`).

## Root cause

The split shape exists on `origin/main` (`WorldStateSenderTest.cpp`, `GameEventBroadcasterTest.cpp`, `UdpSocketTest.cpp`) but it is the minority: 106 of the 127 test files hold an anonymous namespace, and 37 `TEST_F` fixtures are defined inside a test file against 4 in a header of their own. `fix-architecture-magic-values-as-named-constants.md` banned constants "in an anonymous namespace at the top of a `.cpp`" without saying it applies to tests, and its section on test constants only asked to search before defining one. No rule spoke of fixtures. The neighbouring test files were copied.

## Rule

See the rule file of the same name in `.claude/rules/code-style/`: a test `.cpp` holds includes, `using` lines and tests only; constants in `<Subject>TestConstants.hpp`; `TEST_F` classes in `<Subject>Fixture.{hpp,cpp}`, scenes as fixtures or classes of their own, helpers in `<Subject>Fixtures.{hpp,cpp}`, doubles in `tests/doubles/`; a changed test brought to that shape whole; a grep calibrated on `CameraShakeTest.cpp` and `WorldStateSenderTest.cpp`.

## Example

- ❌ **Before (wrong)**: `CameraShakeTest.cpp` with eight constants, `struct ShakeOverASecond` and `shakeKeeping()` in an anonymous namespace.
- ✅ **After (right)**: `CameraShakeTestConstants.hpp` and `CameraShakeFixtures.{hpp,cpp}` next to the test, `CameraShakeFixtures.cpp` listed in the sources of `client_effect_tests`, and the test file holding its tests only.
