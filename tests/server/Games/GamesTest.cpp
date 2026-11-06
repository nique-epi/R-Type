#include <gtest/gtest.h>
#include <cstddef>
#include "GamesFixture.hpp"
#include "PlayerArrival.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerException.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::game::PlayerArrival;
using rtype::server::NoJoinableGameException;
using rtype::server::testing::playerEndpoint;

/**
 * Given no game was added
 * When a new player asks which game to join
 * Then a NoJoinableGameException is thrown
 */
TEST_F(GamesFixture, HasNoGameToJoinBeforeOneIsAdded) {
  EXPECT_THROW(static_cast<void>(games().joinable()), NoJoinableGameException);
}

/**
 * Given two games waiting for players, the first added before the second
 * When a new player asks which game to join
 * Then it is the first game
 */
TEST_F(GamesFixture, LetsNewPlayersJoinTheFirstGameWhileEveryGameWaits) {
  addGame();
  addGame();

  EXPECT_EQ(&games().joinable(), games().all()[FIRST_GAME].get());
}

/**
 * Given two games waiting for players, the first with every seat taken
 * When a new player asks which game to join
 * Then it is the second game
 */
TEST_F(GamesFixture, SendsNewPlayersToTheNextGameOnceTheFirstIsFull) {
  addGame();
  addGame();
  fill(*games().all()[FIRST_GAME]);

  EXPECT_EQ(&games().joinable(), games().all()[SECOND_GAME].get());
}

/**
 * Given two games, both with every seat taken
 * When a new player asks which game to join
 * Then it is the first game, which refuses it
 */
TEST_F(GamesFixture, FallsBackToTheFirstGameWhenNoneHasASeat) {
  addGame();
  addGame();
  fill(*games().all()[FIRST_GAME]);
  fill(*games().all()[SECOND_GAME]);

  EXPECT_EQ(&games().joinable(), games().all()[FIRST_GAME].get());
}

/**
 * Given a player seated in the second game, while the first has free seats
 * When it sends a connection request
 * Then the request goes to the second game, its own
 */
TEST_F(GamesFixture, PlacesASeatedPlayerInItsOwnGame) {
  addGame();
  addGame();
  joinIn(*games().all()[SECOND_GAME], 0);

  EXPECT_EQ(games().placeOf(playerEndpoint(0)), SECOND_GAME);
}

/**
 * Given a first game with every seat taken, and a second game waiting
 * When a player seated in no game sends a connection request
 * Then the request goes to the second game, the one new players join
 */
TEST_F(GamesFixture, PlacesAStrangerInTheGameNewPlayersJoin) {
  addGame();
  addGame();
  fill(*games().all()[FIRST_GAME]);

  EXPECT_EQ(games().placeOf(playerEndpoint(rtype::network::MAXIMUM_PLAYERS)),
            SECOND_GAME);
}

/**
 * Given two games, and a subscriber to the arrivals on the second bus listed
 * When a player joins the second game
 * Then the subscriber hears that arrival: the buses follow the games in order
 */
TEST_F(GamesFixture, ListsTheBusOfEachGameInTheOrderOfTheGames) {
  addGame();
  addGame();
  std::size_t arrivals = 0;
  games().buses()[SECOND_GAME]->subscribe<PlayerArrival>(
      [&arrivals]([[maybe_unused]] const PlayerArrival& arrival) {
        ++arrivals;
      });

  joinIn(*games().all()[SECOND_GAME], 0);
  games().dispatchEvents();

  EXPECT_EQ(arrivals, 1U);
}
