#include <gtest/gtest.h>
#include <cstddef>
#include <initializer_list>
#include <vector>
#include "BydoGunnerFixture.hpp"
#include "BydoGunnerTestConstants.hpp"
#include "EntitySpawned.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

/**
 * Given a Bydo that just appeared
 * When the ticks of one and a half seconds but one run
 * Then it fired nothing yet
 */
TEST_F(BydoGunnerFixture, FiresNothingBeforeOneAndAHalfSeconds) {
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS - 1);

  EXPECT_TRUE(enemyMissilesInTheWorld().empty());
}

/**
 * Given a Bydo that just appeared
 * When the ticks of one and a half seconds run
 * Then it fired one missile
 */
TEST_F(BydoGunnerFixture, FiresOneAndAHalfSecondsAfterItAppears) {
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  EXPECT_EQ(enemyMissilesInTheWorld().size(), 1U);
}

/**
 * Given a Bydo that just fired its first missile
 * When the ticks of one and a half seconds but one run
 * Then it fired no second missile yet
 */
TEST_F(BydoGunnerFixture, WaitsOneAndAHalfSecondsBetweenTwoMissiles) {
  bydoAt(SOME_BYDO);
  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS - 1);

  EXPECT_EQ(enemyMissilesInTheWorld().size(), 1U);
}

/**
 * Given a Bydo that just fired its first missile
 * When the ticks of one and a half seconds run again
 * Then it fired a second missile
 */
TEST_F(BydoGunnerFixture, FiresAgainEveryOneAndAHalfSeconds) {
  bydoAt(SOME_BYDO);
  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  EXPECT_EQ(enemyMissilesInTheWorld().size(), 2U);
}

/**
 * Given a Bydo, and a second one that appeared half a second later
 * When one and a half seconds passed since the first appeared
 * Then only the first fired: each Bydo counts from its own appearance
 */
TEST_F(BydoGunnerFixture, EachBydoCountsFromItsOwnAppearance) {
  bydoAt(SOME_BYDO);
  runTicks(HALF_A_SECOND_IN_TICKS);
  bydoAt(OTHER_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS - HALF_A_SECOND_IN_TICKS);

  const std::vector<network::EntitySpawned> missiles =
      enemyMissilesInTheWorld();
  ASSERT_EQ(missiles.size(), 1U);
  EXPECT_FLOAT_EQ(missiles.front().y, SOME_BYDO.y);
}

/**
 * Given a Bydo
 * When it fires
 * Then its missile starts on its row, its right side against the left side of
 * the Bydo
 */
TEST_F(BydoGunnerFixture, FiresFromTheFrontOfTheBydo) {
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  const std::vector<network::EntitySpawned> missiles =
      enemyMissilesInTheWorld();
  ASSERT_EQ(missiles.size(), 1U);
  EXPECT_FLOAT_EQ(missiles.front().x, SOME_BYDO_MISSILE_X);
  EXPECT_FLOAT_EQ(missiles.front().y, SOME_BYDO.y);
}

/**
 * Given two connected players and a Bydo
 * When it fires
 * Then each player is sent, in a datagram of its own, the EntitySpawned of the
 * missile of an enemy, under its network id and at its position
 */
TEST_F(BydoGunnerFixture, TellsEveryPlayerAboutTheMissile) {
  connect(FIRST_PLAYER);
  connect(SECOND_PLAYER);
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  const std::vector<network::EntitySpawned> missiles =
      enemyMissilesInTheWorld();
  ASSERT_EQ(missiles.size(), 1U);
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    const std::vector<network::OutgoingDatagram> received =
        datagramsSentTo(player);
    ASSERT_EQ(received.size(), 1U);
    EXPECT_EQ(onlyMessageIn<network::EntitySpawned>(received.front()),
              missiles.front());
  }
}

}  // namespace rtype::server::testing
