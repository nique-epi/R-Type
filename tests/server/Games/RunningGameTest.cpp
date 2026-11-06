#include <gtest/gtest.h>
#include <cstddef>
#include "PlayerArrival.hpp"
#include "RunningGameFixture.hpp"
#include "ServerException.hpp"

using rtype::game::PlayerArrival;
using rtype::server::GameNotCreatedException;

/**
 * Given no game hosted yet
 * When the game is asked for
 * Then a GameNotCreatedException is thrown
 */
TEST_F(RunningGameFixture, HasNoGameBeforeOneIsHosted) {
  EXPECT_THROW(static_cast<void>(runningGame().game()),
               GameNotCreatedException);
}

/**
 * Given a hosted game, and a subscriber to the arrivals on the bus of
 * runningGame()
 * When a player joins the game and the bus delivers
 * Then the subscriber hears that arrival: the game publishes on that bus
 */
TEST_F(RunningGameFixture, HostsAGameThatPublishesOnItsBus) {
  hostGame();
  std::size_t arrivals = 0;
  runningGame().events().subscribe<PlayerArrival>(
      [&arrivals]([[maybe_unused]] const PlayerArrival& arrival) {
        ++arrivals;
      });

  join(0);
  runningGame().events().dispatch();

  EXPECT_EQ(arrivals, 1U);
}
