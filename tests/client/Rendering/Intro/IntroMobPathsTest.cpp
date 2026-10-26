#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "IntroCastConstants.hpp"
#include "IntroMob.hpp"
#include "IntroMobKind.hpp"
#include "IntroMobPaths.hpp"
#include "IntroTestConstants.hpp"
#include "IntroTimelineConstants.hpp"

using rtype::client::helixHitAt;
using rtype::client::helixMobAt;
using rtype::client::INTRO_BEAT;
using rtype::client::INTRO_HELIX_SIZE;
using rtype::client::INTRO_REINFORCEMENTS;
using rtype::client::INTRO_RING_SIZE;
using rtype::client::INTRO_SWARM_CUT;
using rtype::client::IntroMobKind;
using rtype::client::newcomerCenterAt;
using rtype::client::openingMobsAt;
using rtype::client::testing::A_MOMENT;
using rtype::client::testing::LONGER_THAN_A_FLASH;
using rtype::client::testing::SCREEN_WIDTH;

/**
 * Given the cascade of the helix
 * When the first newcomer arrives
 * Then no Bydo of the helix is left: the ring is made of new Bydos only
 */
TEST(IntroMobPaths, TheWholeHelixIsDeadWhenTheReinforcementsArrive) {
  for (std::size_t bydo = 0; bydo < INTRO_HELIX_SIZE; ++bydo) {
    EXPECT_FALSE(helixMobAt(bydo, INTRO_REINFORCEMENTS).has_value()) << bydo;
  }
}

/**
 * Given the reinforcements
 * When each newcomer arrives
 * Then it comes from past the right edge of the screen
 */
TEST(IntroMobPaths, EveryNewcomerComesFromPastTheRightEdge) {
  for (std::size_t newcomer = 0; newcomer < INTRO_RING_SIZE; ++newcomer) {
    const float arrival =
        INTRO_REINFORCEMENTS +
        (static_cast<float>(newcomer) * rtype::client::INTRO_NEWCOMER_STAGGER);
    EXPECT_GT(newcomerCenterAt(newcomer, arrival).x, SCREEN_WIDTH) << newcomer;
  }
}

/**
 * Given a Bydo of the helix
 * When it is read just before its missile hits it
 * Then it is flashed
 */
TEST(IntroMobPaths, ABydoFlashesJustBeforeItsMissileHits) {
  const std::optional<rtype::client::IntroMob> bydo =
      helixMobAt(0, helixHitAt(0) - A_MOMENT);

  ASSERT_TRUE(bydo.has_value());
  EXPECT_TRUE(bydo.value_or(rtype::client::IntroMob{}).flashed);
}

/**
 * Given a Bydo of the helix
 * When it is read longer than a flash before its missile hits it
 * Then it is not flashed yet
 */
TEST(IntroMobPaths, ABydoIsNotFlashedBeforeItsMissileComes) {
  const std::optional<rtype::client::IntroMob> bydo =
      helixMobAt(0, helixHitAt(0) - LONGER_THAN_A_FLASH);

  ASSERT_TRUE(bydo.has_value());
  EXPECT_FALSE(bydo.value_or(rtype::client::IntroMob{}).flashed);
}

/**
 * Given the Bydo in the dark
 * When it is read just before the first beat and at it
 * Then it shows until the first beat only
 */
TEST(IntroMobPaths, TheBydoInTheDarkLeavesAtTheFirstBeat) {
  EXPECT_TRUE(openingMobsAt(INTRO_BEAT - A_MOMENT)[0].has_value());
  EXPECT_FALSE(openingMobsAt(INTRO_BEAT)[0].has_value());
}

/**
 * Given the helix
 * When its last two Bydos and the one before are read
 * Then the last two are bonus Bydos and the one before is not
 */
TEST(IntroMobPaths, TheLastTwoBydosOfTheHelixAreBonusBydos) {
  const float scene = INTRO_SWARM_CUT + A_MOMENT;
  const std::optional<rtype::client::IntroMob> last =
      helixMobAt(INTRO_HELIX_SIZE - 1, scene);
  const std::optional<rtype::client::IntroMob> beforeLast =
      helixMobAt(INTRO_HELIX_SIZE - 2, scene);
  const std::optional<rtype::client::IntroMob> third =
      helixMobAt(INTRO_HELIX_SIZE - 3, scene);

  ASSERT_TRUE(last.has_value() && beforeLast.has_value() && third.has_value());
  EXPECT_EQ(last.value_or(rtype::client::IntroMob{}).kind,
            IntroMobKind::BonusBydo);
  EXPECT_EQ(beforeLast.value_or(rtype::client::IntroMob{}).kind,
            IntroMobKind::BonusBydo);
  EXPECT_EQ(third.value_or(rtype::client::IntroMob{}).kind, IntroMobKind::Bydo);
}
