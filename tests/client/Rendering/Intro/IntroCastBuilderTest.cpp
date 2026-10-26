#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "IntroCast.hpp"
#include "IntroCastBuilder.hpp"
#include "IntroCastConstants.hpp"
#include "IntroMissile.hpp"
#include "IntroMobPaths.hpp"
#include "IntroShipPaths.hpp"
#include "IntroTestConstants.hpp"
#include "IntroTimelineConstants.hpp"
#include "SquadronConstants.hpp"

using rtype::client::buildIntroCast;
using rtype::client::helixCenterAt;
using rtype::client::helixHitAt;
using rtype::client::INTRO_HELIX_SIZE;
using rtype::client::INTRO_HOMES;
using rtype::client::INTRO_RING_CENTER;
using rtype::client::INTRO_SCENE_RELEASE;
using rtype::client::IntroCast;
using rtype::client::IntroMissile;
using rtype::client::shipPlaceAt;
using rtype::client::SQUADRON_MUZZLE;
using rtype::client::testing::INTRO_TEST_SEED;

/**
 * Given a cast
 * When the missiles of the players are read
 * Then each Bydo of the helix is hit, where it is, when the cascade says
 */
TEST(IntroCastBuilder, EveryBydoOfTheHelixIsHitWhereItIs) {
  const IntroCast cast = buildIntroCast(INTRO_TEST_SEED);

  for (std::size_t bydo = 0; bydo < INTRO_HELIX_SIZE; ++bydo) {
    const float hit = helixHitAt(bydo);
    EXPECT_TRUE(std::ranges::any_of(cast.missiles, [&](const IntroMissile&
                                                           missile) {
      return !missile.enemy && missile.end == hit &&
             missile.to == helixCenterAt(bydo, hit);
    })) << bydo;
  }
}

/**
 * Given a cast
 * When the missiles of the players are read
 * Then each leaves the muzzle of a ship, where that ship is when it fires
 */
TEST(IntroCastBuilder, EveryMissileLeavesTheMuzzleOfAShip) {
  const IntroCast cast = buildIntroCast(INTRO_TEST_SEED);

  for (const IntroMissile& missile : cast.missiles) {
    if (missile.enemy) {
      continue;
    }
    bool fromAShip = false;
    for (std::size_t pilot = 0; pilot < INTRO_HOMES.size(); ++pilot) {
      const std::optional<sf::Vector2f> place =
          shipPlaceAt(pilot, missile.start);
      fromAShip = fromAShip || (place.has_value() &&
                                *place + SQUADRON_MUZZLE == missile.from);
    }
    EXPECT_TRUE(fromAShip) << missile.start;
  }
}

/**
 * Given two casts of the same seed
 * When their bursts are compared
 * Then they throw the same sparks: a run replays the same intro
 */
TEST(IntroCastBuilder, OneSeedGivesOneCast) {
  const IntroCast first = buildIntroCast(INTRO_TEST_SEED);
  const IntroCast second = buildIntroCast(INTRO_TEST_SEED);

  ASSERT_EQ(first.bursts.size(), second.bursts.size());
  for (std::size_t burst = 0; burst < first.bursts.size(); ++burst) {
    EXPECT_EQ(first.bursts[burst].sparks, second.bursts[burst].sparks);
  }
}

/**
 * Given a cast
 * When its bursts are read
 * Then the ring bursts at its center 30 ms after the beams leave
 */
TEST(IntroCastBuilder, TheRingBurstsJustAfterTheBeams) {
  const IntroCast cast = buildIntroCast(INTRO_TEST_SEED);

  EXPECT_TRUE(std::ranges::any_of(cast.bursts, [](const auto& burst) {
    return burst.center == INTRO_RING_CENTER &&
           burst.start ==
               INTRO_SCENE_RELEASE + rtype::client::INTRO_BURST_DELAY;
  }));
}
