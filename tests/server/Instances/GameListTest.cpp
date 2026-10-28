#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameList.hpp"
#include "GameListFixtures.hpp"
#include "ServerException.hpp"

using rtype::engine::EventBus;
using rtype::server::GameInstance;
using rtype::server::GameList;
using rtype::server::NoGameException;
using rtype::server::testing::silentGame;

/**
 * Given a list of two games
 * When the game a new player joins is asked for
 * Then it is the first game added
 */
TEST(GameList, SeatsNewPlayersInTheFirstGame) {
  EventBus events;
  GameList games;
  std::unique_ptr<GameInstance> first = silentGame(events);
  const GameInstance* firstGame = first.get();
  games.add(std::move(first));
  games.add(silentGame(events));

  const GameInstance& joinable = games.joinable();

  EXPECT_EQ(&joinable, firstGame);
}

/**
 * Given a list that holds no game
 * When the game a new player joins is asked for
 * Then the list refuses with NoGameException
 */
TEST(GameList, RefusesToNameAGameWhenItHoldsNone) {
  const GameList games;

  EXPECT_THROW(static_cast<void>(games.joinable()), NoGameException);
}
