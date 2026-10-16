#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server {
namespace {

using testing::FIVE_PLAYER_PORTS;
using testing::OTHER_SESSION_TOKEN;
using testing::PLAYER_ADDRESS;
using testing::PLAYER_NICKNAME;
using testing::SESSION_TOKEN;

network::Endpoint playerEndpoint(std::size_t player) {
  return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                           .port = FIVE_PLAYER_PORTS.at(player)};
}

/** @brief Adds players 0 to @p count - 1 of FIVE_PLAYER_PORTS. */
void addPlayers(PlayerRoster& roster, std::size_t count) {
  for (std::size_t player = 0; player < count; ++player) {
    static_cast<void>(roster.add(playerEndpoint(player),
                                 std::string{PLAYER_NICKNAME}, SESSION_TOKEN));
  }
}

/**
 * Given an empty roster
 * When a sender is looked up
 * Then nobody is found
 */
TEST(PlayerRoster, FindsNobodyWhenEmpty) {
  PlayerRoster roster;

  EXPECT_EQ(roster.find(playerEndpoint(0)), nullptr);
}

/**
 * Given an empty roster
 * When four players join one after the other
 * Then they are numbered 1, 2, 3 and 4 in the order they joined
 */
TEST(PlayerRoster, NumbersThePlayersInTheOrderTheyJoin) {
  PlayerRoster roster;

  for (std::size_t player = 0; player < network::MAXIMUM_PLAYERS; ++player) {
    const ConnectedPlayer* const added = roster.add(
        playerEndpoint(player), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
    ASSERT_NE(added, nullptr);
    EXPECT_EQ(added->playerNumber, network::FIRST_PLAYER_NUMBER + player);
  }
}

/**
 * Given a roster holding one player fewer than the maximum
 * When it is asked whether it is full
 * Then it is not
 */
TEST(PlayerRoster, IsNotFullWithOnePlaceLeft) {
  PlayerRoster roster;
  addPlayers(roster, network::MAXIMUM_PLAYERS - 1);

  EXPECT_FALSE(roster.isFull());
}

/**
 * Given a roster holding the maximum number of players
 * When it is asked whether it is full
 * Then it is
 */
TEST(PlayerRoster, IsFullWithTheMaximumNumberOfPlayers) {
  PlayerRoster roster;
  addPlayers(roster, network::MAXIMUM_PLAYERS);

  EXPECT_TRUE(roster.isFull());
}

/**
 * Given a full roster
 * When a fifth sender is added
 * Then nobody is added and the sender is not found
 */
TEST(PlayerRoster, AddsNoFifthPlayer) {
  PlayerRoster roster;
  addPlayers(roster, network::MAXIMUM_PLAYERS);
  const network::Endpoint fifth = playerEndpoint(network::MAXIMUM_PLAYERS);

  EXPECT_EQ(roster.add(fifth, std::string{PLAYER_NICKNAME}, SESSION_TOKEN),
            nullptr);
  EXPECT_EQ(roster.find(fifth), nullptr);
}

/**
 * Given a player connected with a session token
 * When the same endpoint is added again with another token
 * Then the player keeps its number and its first token
 */
TEST(PlayerRoster, KeepsAConnectedPlayerAsItWas) {
  PlayerRoster roster;
  addPlayers(roster, 1);

  const ConnectedPlayer* const again = roster.add(
      playerEndpoint(0), std::string{PLAYER_NICKNAME}, OTHER_SESSION_TOKEN);

  ASSERT_NE(again, nullptr);
  EXPECT_EQ(again->playerNumber, network::FIRST_PLAYER_NUMBER);
  EXPECT_EQ(again->sessionToken, SESSION_TOKEN);
}

/**
 * Given a roster
 * When a player joins
 * Then it is found at its endpoint, with its nickname, its token and the
 * sequence 0 for the first datagram sent to it
 */
TEST(PlayerRoster, FindsAPlayerWithWhatItJoinedWith) {
  PlayerRoster roster;
  addPlayers(roster, 1);

  const ConnectedPlayer* const found = roster.find(playerEndpoint(0));

  ASSERT_NE(found, nullptr);
  EXPECT_EQ(found->nickname, PLAYER_NICKNAME);
  EXPECT_EQ(found->sessionToken, SESSION_TOKEN);
  EXPECT_EQ(found->nextSequence, 0);
}

}  // namespace
}  // namespace rtype::server
