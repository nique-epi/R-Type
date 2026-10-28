#include <gtest/gtest.h>
#include "GamesFixture.hpp"
#include "ServerException.hpp"

using rtype::server::NoJoinableGameException;

/**
 * Given no game was added
 * When a new player asks which game to join
 * Then a NoJoinableGameException is thrown
 */
TEST_F(GamesFixture, HasNoGameToJoinBeforeOneIsAdded) {
  EXPECT_THROW(static_cast<void>(games_.joinable()), NoJoinableGameException);
}

/**
 * Given two games, the first added before the second
 * When a new player asks which game to join
 * Then it is the first game
 */
TEST_F(GamesFixture, LetsNewPlayersJoinTheFirstGame) {
  addGame();
  addGame();

  EXPECT_EQ(&games_.joinable(), games_.all()[0].get());
  EXPECT_NE(&games_.joinable(), games_.all()[1].get());
}
