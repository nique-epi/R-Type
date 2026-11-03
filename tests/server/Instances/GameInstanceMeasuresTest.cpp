#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include "GameInstance.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceMeasuresTestConstants.hpp"
#include "GameInstanceTestConstants.hpp"
#include "SystemScheduler.hpp"
#include "SystemTime.hpp"

namespace rtype::server {

using testing::FIRST_PLAYER;
using testing::GameInstanceTest;
using testing::NAMES_OF_THE_SYSTEMS;

/**
 * Given a game no player joined
 * When its entities are counted
 * Then there is none
 */
TEST_F(GameInstanceTest, AGameNoPlayerJoinedHoldsNoEntity) {
  EXPECT_EQ(game().entityCount(), 0U);
}

/**
 * Given a game no player joined
 * When a player joins
 * Then the game holds one entity, the ship of that player
 */
TEST_F(GameInstanceTest, TheShipOfAPlayerIsAnEntityOfTheGame) {
  join(FIRST_PLAYER);

  EXPECT_EQ(game().entityCount(), 1U);
}

/**
 * Given a game
 * When the times of its systems are taken
 * Then each system of an R-Type game is listed under its name, in the order
 * they run
 */
TEST_F(GameInstanceTest, NamesEachOfItsSystems) {
  const std::vector<engine::SystemTime> times =
      game().systems().takeSystemTimes();

  EXPECT_TRUE(std::ranges::equal(times, NAMES_OF_THE_SYSTEMS, {},
                                 &engine::SystemTime::name));
}

}  // namespace rtype::server
