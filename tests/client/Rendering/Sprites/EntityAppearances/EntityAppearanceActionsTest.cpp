#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <string_view>
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "EngineException.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearanceActionsTestConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntityAppearancesFixtures.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "FrameAnchor.hpp"
#include "Playback.hpp"

using rtype::client::ActionAnimation;
using rtype::client::EntityAppearance;
using rtype::client::EntityAppearances;
using rtype::client::FrameAnchor;
using rtype::client::Playback;
using rtype::engine::DataFileValueOutOfRangeException;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnknownDataFileChoiceException;
using rtype::engine::UnknownDataFileKeyException;
using rtype::game::ActionKey;
using rtype::game::EnemyPhase;
using ::testing::ElementsAreArray;

namespace {

/** @returns The animation @p appearance gives the attack @p attack in the
 * phase @p phase, or nothing when it gives none. */
const ActionAnimation* actionOf(const EntityAppearance& appearance,
                                std::uint8_t attack, EnemyPhase phase) {
  const ActionKey key{.attack = attack, .phase = phase};
  const auto found =
      std::ranges::find(appearance.actions, key, &ActionAnimation::key);
  return found == appearance.actions.end() ? nullptr : &*found;
}

/** @returns The strike of the first attack of the sheet named
 * READ_TYPE_NAME in @p appearances; an empty one, and a failure, without. */
const ActionAnimation& strikeOf(const EntityAppearances& appearances) {
  static const ActionAnimation none{};
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  EXPECT_NE(read, nullptr);
  const ActionAnimation* const strike =
      read == nullptr ? nullptr
                      : actionOf(*read, FIRST_ATTACK, EnemyPhase::Strike);
  EXPECT_NE(strike, nullptr);
  return strike == nullptr ? none : *strike;
}

}  // namespace

/**
 * Given a sheet with the strike of its first attack
 * When its appearance is read
 * Then the strike has its four frames along their row
 */
TEST(EntityAppearanceActions, ReadsTheFramesOfAnAttackPhase) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_ATTACKS_TEXT);

  EXPECT_THAT(strikeOf(appearances).frames, ElementsAreArray(STRIKE_FRAMES));
}

/**
 * Given a sheet whose strike shows 80 ms per frame and holds its last frame
 * When its appearance is read
 * Then the strike lasts 80 ms per frame and holds
 */
TEST(EntityAppearanceActions, ReadsTheFrameTimeAndThePlayback) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_ATTACKS_TEXT);

  EXPECT_EQ(strikeOf(appearances).frameDuration, STRIKE_FRAME_TIME);
  EXPECT_EQ(strikeOf(appearances).playback, Playback::Hold);
}

/**
 * Given a sheet whose strike hangs 12 pixels to the left of the body
 * When its appearance is read
 * Then the strike is anchored at x minus 12 and y 0
 */
TEST(EntityAppearanceActions, ReadsTheAnchorOfAnAttackPhase) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_ATTACKS_TEXT);

  EXPECT_EQ(strikeOf(appearances).anchor, (FrameAnchor{.x = -12, .y = 0}));
}

/**
 * Given a sheet whose first attack goes back to the idle animation and whose
 * second one stays on its last frame
 * When its appearance is read
 * Then the first returns to idle and the second does not
 */
TEST(EntityAppearanceActions, ReadsWhatHappensAfterwards) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_ATTACKS_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);
  const ActionAnimation* const signal =
      actionOf(*read, SECOND_ATTACK, EnemyPhase::Signal);
  ASSERT_NE(signal, nullptr);

  EXPECT_TRUE(strikeOf(appearances).returnsToIdle);
  EXPECT_FALSE(signal->returnsToIdle);
  EXPECT_EQ(signal->anchor, FrameAnchor{});
}

/**
 * Given a strike that leaves `afterwards` out
 * When its appearance is read
 * Then it goes back to the idle animation
 */
TEST(EntityAppearanceActions, ReturnsToIdleByDefault) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITHOUT_AFTERWARDS_TEXT);

  EXPECT_TRUE(strikeOf(appearances).returnsToIdle);
}

/**
 * Given a sheet that describes only the strike of the first attack
 * When its appearance is read
 * Then no other phase of that attack has an animation
 */
TEST(EntityAppearanceActions, GivesNoAnimationToAPhaseTheSheetLeavesOut) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITHOUT_AFTERWARDS_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_EQ(actionOf(*read, FIRST_ATTACK, EnemyPhase::Signal), nullptr);
  EXPECT_EQ(actionOf(*read, SECOND_ATTACK, EnemyPhase::Strike), nullptr);
}

/**
 * Given a sheet with no attack table
 * When its appearance is read
 * Then it has no action animation
 */
TEST(EntityAppearanceActions, ASheetWithoutAttacksHasNoAction) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_TRUE(read->actions.empty());
}

/**
 * Given an attack table whose name has no number
 * When its appearance is read
 * Then it throws, naming the unknown key
 */
TEST(EntityAppearanceActions, RefusesAnAttackWithoutANumber) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(SHEET_WITH_A_NAMELESS_ATTACK_TEXT)),
      UnknownDataFileKeyException);
}

/**
 * Given an attack numbered 256, one past the largest number
 * When its appearance is read
 * Then it throws, naming the unknown key
 */
TEST(EntityAppearanceActions, RefusesAnAttackNumberPastTheLargest) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(SHEET_WITH_ATTACK_256_TEXT)),
      UnknownDataFileKeyException);
}

/**
 * Given an attack with a phase no enemy has
 * When its appearance is read
 * Then it throws, naming the unknown key
 */
TEST(EntityAppearanceActions, RefusesAnUnknownPhase) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(SHEET_WITH_AN_UNKNOWN_PHASE_TEXT)),
      UnknownDataFileKeyException);
}

/**
 * Given a phase without `playback`
 * When its appearance is read
 * Then it throws, naming the missing key
 */
TEST(EntityAppearanceActions, RefusesAPhaseWithoutAPlayback) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(SHEET_WITHOUT_PLAYBACK_TEXT)),
      MissingDataFileKeyException);
}

/**
 * Given a phase whose playback is `bounce`
 * When its appearance is read
 * Then it throws, listing the playbacks
 */
TEST(EntityAppearanceActions, RefusesAnUnknownPlayback) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(SHEET_WITH_AN_UNKNOWN_PLAYBACK_TEXT)),
               UnknownDataFileChoiceException);
}

/**
 * Given a phase whose `afterwards` is `forever`
 * When its appearance is read
 * Then it throws, listing the choices
 */
TEST(EntityAppearanceActions, RefusesAnUnknownAfterwards) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(SHEET_WITH_AN_UNKNOWN_AFTERWARDS_TEXT)),
               UnknownDataFileChoiceException);
}

/**
 * Given an anchor 257 pixels to the left, one past the farthest
 * When its appearance is read
 * Then it throws, naming the value out of range
 */
TEST(EntityAppearanceActions, RefusesAnAnchorPastTheFarthest) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(SHEET_WITH_A_FAR_ANCHOR_TEXT)),
      DataFileValueOutOfRangeException);
}

/**
 * Given a phase with a key no animation reads
 * When its appearance is read
 * Then it throws, naming the unknown key
 */
TEST(EntityAppearanceActions, RefusesAnUnknownKeyOfAPhase) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(SHEET_WITH_AN_UNKNOWN_ACTION_KEY_TEXT)),
               UnknownDataFileKeyException);
}
