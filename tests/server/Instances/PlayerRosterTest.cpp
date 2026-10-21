#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
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
 * Given an empty roster
 * When the connected players are listed
 * Then the list is empty
 */
TEST(PlayerRoster, ListsNobodyWhenEmpty) {
  PlayerRoster roster;

  EXPECT_TRUE(roster.connected().empty());
}

/**
 * Given a roster with three players
 * When the connected players are listed
 * Then the three are listed in the order of their numbers
 */
TEST(PlayerRoster, ListsTheConnectedPlayersInOrderOfTheirNumbers) {
  PlayerRoster roster;
  addPlayers(roster, 3);

  const std::vector<ConnectedPlayer*> players = roster.connected();

  ASSERT_EQ(players.size(), 3U);
  EXPECT_EQ(players.at(0)->playerNumber, 1);
  EXPECT_EQ(players.at(1)->playerNumber, 2);
  EXPECT_EQ(players.at(2)->playerNumber, 3);
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

/**
 * Given a roster of two players and an empty one
 * When a player is looked up by number
 * Then the connected numbers find their player, and the number of nobody, 0 and
 * a number above the maximum find nothing
 */
TEST(PlayerRoster, FindsAPlayerByItsNumberOnly) {
  PlayerRoster roster;
  EXPECT_EQ(roster.findByNumber(network::FIRST_PLAYER_NUMBER), nullptr);
  addPlayers(roster, 2);

  const ConnectedPlayer* const first =
      roster.findByNumber(network::FIRST_PLAYER_NUMBER);
  const ConnectedPlayer* const second =
      roster.findByNumber(network::FIRST_PLAYER_NUMBER + 1);

  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(first->endpoint, playerEndpoint(0));
  EXPECT_EQ(second->endpoint, playerEndpoint(1));
  EXPECT_EQ(roster.findByNumber(network::FIRST_PLAYER_NUMBER + 2), nullptr);
  EXPECT_EQ(roster.findByNumber(network::NO_PLAYER_NUMBER), nullptr);
  EXPECT_EQ(roster.findByNumber(network::MAXIMUM_PLAYERS + 1), nullptr);
}

/**
 * Given an empty roster
 * When a sender is removed
 * Then nobody is removed
 */
TEST(PlayerRoster, RemovesNobodyWhenEmpty) {
  PlayerRoster roster;

  EXPECT_FALSE(roster.remove(playerEndpoint(0)).has_value());
}

/**
 * Given a roster with three players
 * When the second is removed
 * Then it comes back as it was, numbered 2
 */
TEST(PlayerRoster, HandsBackTheRemovedPlayer) {
  PlayerRoster roster;
  addPlayers(roster, 3);

  const std::optional<ConnectedPlayer> removed =
      roster.remove(playerEndpoint(1));

  ASSERT_TRUE(removed.has_value());
  const ConnectedPlayer player = removed.value_or(ConnectedPlayer{});
  EXPECT_EQ(player.endpoint, playerEndpoint(1));
  EXPECT_EQ(player.playerNumber, network::FIRST_PLAYER_NUMBER + 1);
}

/**
 * Given a roster with three players
 * When the second is removed
 * Then it is no longer found, and the others are still listed
 */
TEST(PlayerRoster, NoLongerListsARemovedPlayer) {
  PlayerRoster roster;
  addPlayers(roster, 3);

  static_cast<void>(roster.remove(playerEndpoint(1)));

  EXPECT_EQ(roster.find(playerEndpoint(1)), nullptr);
  std::vector<std::uint8_t> numbers;
  for (const ConnectedPlayer* const player : roster.connected()) {
    numbers.push_back(player->playerNumber);
  }
  EXPECT_EQ(numbers,
            (std::vector<std::uint8_t>{network::FIRST_PLAYER_NUMBER,
                                       network::FIRST_PLAYER_NUMBER + 2}));
}

/**
 * Given a full roster
 * When one player is removed
 * Then the roster is no longer full
 */
TEST(PlayerRoster, IsNotFullOnceAPlayerIsRemoved) {
  PlayerRoster roster;
  addPlayers(roster, network::MAXIMUM_PLAYERS);

  static_cast<void>(roster.remove(playerEndpoint(1)));

  EXPECT_FALSE(roster.isFull());
}

/**
 * Given a full roster whose second player was removed
 * When a fifth sender joins
 * Then it takes the number the removed player had, 2
 */
TEST(PlayerRoster, GivesTheNumberOfARemovedPlayerToTheNextOne) {
  PlayerRoster roster;
  addPlayers(roster, network::MAXIMUM_PLAYERS);
  static_cast<void>(roster.remove(playerEndpoint(1)));

  const ConnectedPlayer* const joined =
      roster.add(playerEndpoint(network::MAXIMUM_PLAYERS),
                 std::string{PLAYER_NICKNAME}, OTHER_SESSION_TOKEN);

  ASSERT_NE(joined, nullptr);
  EXPECT_EQ(joined->playerNumber, network::FIRST_PLAYER_NUMBER + 1);
}

/**
 * Given a roster with two players
 * When a sender that is not connected is removed
 * Then nobody is removed and both players are still listed
 */
TEST(PlayerRoster, RemovesNobodyForASenderNotConnected) {
  PlayerRoster roster;
  addPlayers(roster, 2);

  const std::optional<ConnectedPlayer> removed =
      roster.remove(playerEndpoint(network::MAXIMUM_PLAYERS));

  EXPECT_FALSE(removed.has_value());
  EXPECT_EQ(roster.connected().size(), 2);
}

/**
 * Given a roster of three players whose second was removed
 * When the addresses of the players are listed
 * Then the first and the third are listed, in order of their numbers
 */
TEST(PlayerRoster, ListsTheAddressesOfTheConnectedPlayersInOrder) {
  PlayerRoster roster;
  addPlayers(roster, 3);
  static_cast<void>(roster.remove(playerEndpoint(1)));

  EXPECT_EQ(roster.endpoints(),
            (std::vector{playerEndpoint(0), playerEndpoint(2)}));
}

}  // namespace
}  // namespace rtype::server
