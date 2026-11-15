#include <gtest/gtest.h>
#include <vector>
#include "GameCasinoFixtures.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceTestConstants.hpp"
#include "LobbyState.hpp"
#include "MessageType.hpp"

namespace rtype::server::testing {

/**
 * Given two seated players in the waiting room
 * When a datagram holding a CasinoPlace at the table arrives from the first one
 * Then the room shows the first player at the casino, and the first alone gets
 * the state of the roulette table: the handler is registered, the casino
 * module is in the list of the game, and the place goes through the checks of
 * a real datagram
 */
TEST_F(GameInstanceTest, SeatsAPlayerAtTheTableThroughARealDatagram) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  queue(casinoPlaceFrom(FIRST_PLAYER));

  tick();

  const std::vector<network::LobbyState> rooms =
      lobbyStatesIn(sentTo(SECOND_PLAYER));
  ASSERT_FALSE(rooms.empty());
  ASSERT_FALSE(rooms.back().players.empty());
  EXPECT_EQ(rooms.back().players.front().place, network::PilotPlace::AtCasino);
  EXPECT_FALSE(ofType(network::MessageType::RouletteRound, sentTo(FIRST_PLAYER))
                   .empty());
  EXPECT_TRUE(ofType(network::MessageType::RouletteRound, sentTo(SECOND_PLAYER))
                  .empty());
}

}  // namespace rtype::server::testing
