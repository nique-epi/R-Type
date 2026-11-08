#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "EntitySpawned.hpp"
#include "Position.hpp"
#include "VolleySystemFixture.hpp"
#include "VolleySystemTestConstants.hpp"

using rtype::game::Position;
using rtype::network::EntitySpawned;
using rtype::server::testing::BROOD_SIGNAL;
using rtype::server::testing::BROOD_STRIKE;
using rtype::server::testing::FIRST_VOLLEY_PLAYER;
using rtype::server::testing::MINION_COLUMN;
using rtype::server::testing::MINIONS_PER_VOLLEY;
using rtype::server::testing::SECOND_VOLLEY_PLAYER;
using rtype::server::testing::VOLLEY_PLAYER_COUNT;
using rtype::server::testing::VolleySystemFixture;
using ::testing::Each;
using ::testing::Field;

/**
 * Given a brooder that starts the strike its sheet fires a column of three
 * minions on
 * When the volley system runs one tick
 * Then the three minions appear 100 units to its left, 50 units apart going
 * down
 */
TEST_F(VolleySystemFixture, FiresTheVolleyOfTheActionStarted) {
  addBrooder();
  startAction(BROOD_STRIKE);

  runTick();

  const std::vector<Position> found = minions();
  ASSERT_EQ(found.size(), MINIONS_PER_VOLLEY);
  for (std::size_t index = 0; index < MINIONS_PER_VOLLEY; ++index) {
    EXPECT_FLOAT_EQ(found.at(index).x, MINION_COLUMN.at(index).x);
    EXPECT_FLOAT_EQ(found.at(index).y, MINION_COLUMN.at(index).y);
  }
}

/**
 * Given a brooder that starts the signal of the attack its volley is fired
 * in the strike of
 * When the volley system runs one tick
 * Then no minion appears
 */
TEST_F(VolleySystemFixture, FiresNothingInAnotherAction) {
  addBrooder();
  startAction(BROOD_SIGNAL);

  runTick();

  EXPECT_TRUE(minions().empty());
}

/**
 * Given a brooder that starts no action
 * When the volley system runs one tick
 * Then no minion appears
 */
TEST_F(VolleySystemFixture, FiresNothingWithoutAStartedAction) {
  addBrooder();

  runTick();

  EXPECT_TRUE(minions().empty());
}

/**
 * Given two connected players and a brooder that starts its strike
 * When the volley system runs one tick
 * Then each player is sent one EntitySpawned of a minion for each body
 */
TEST_F(VolleySystemFixture, TellsEveryPlayerAboutEachBody) {
  connectPlayers(VOLLEY_PLAYER_COUNT);
  addBrooder();
  startAction(BROOD_STRIKE);

  runTick();

  for (const auto player : {FIRST_VOLLEY_PLAYER, SECOND_VOLLEY_PLAYER}) {
    const std::vector<EntitySpawned> spawns = spawnsSentTo(player);
    EXPECT_EQ(spawns.size(), MINIONS_PER_VOLLEY);
    EXPECT_THAT(spawns,
                Each(Field(&EntitySpawned::entityType, minionNumber())));
  }
}

/**
 * Given a brooder that starts its strike
 * When the volley system runs one tick
 * Then the world holds four enemies: a minion of a volley counts as one
 */
TEST_F(VolleySystemFixture, ABydoOfAVolleyIsAnEnemy) {
  addBrooder();
  startAction(BROOD_STRIKE);

  runTick();

  EXPECT_EQ(enemies(), 1 + MINIONS_PER_VOLLEY);
}
