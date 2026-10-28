#include <gtest/gtest.h>
#include <array>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {
namespace {

/**
 * Given a list nothing was appended to
 * When it is read
 * Then it is empty, and its first player is its end
 */
TEST(ConnectedPlayers, IsEmptyWhenNothingWasAppended) {
  const ConnectedPlayers players;

  EXPECT_TRUE(players.empty());
  EXPECT_EQ(players.size(), 0U);
  EXPECT_EQ(players.begin(), players.end());
}

/**
 * Given MAXIMUM_PLAYERS players
 * When each is appended in turn
 * Then the list holds every one of them, in the order they were appended
 */
TEST(ConnectedPlayers, ListsThePlayersInTheOrderTheyWereAppended) {
  std::array<ConnectedPlayer, network::MAXIMUM_PLAYERS> seated{};
  std::vector<ConnectedPlayer*> appended;
  ConnectedPlayers players;

  for (ConnectedPlayer& player : seated) {
    players.append(&player);
    appended.push_back(&player);
  }

  EXPECT_FALSE(players.empty());
  EXPECT_EQ(players.size(), network::MAXIMUM_PLAYERS);
  EXPECT_EQ(std::vector<ConnectedPlayer*>(players.begin(), players.end()),
            appended);
}

}  // namespace
}  // namespace rtype::server
