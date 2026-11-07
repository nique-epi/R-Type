#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionRefused.hpp"
#include "Endpoint.hpp"
#include "EntityTypeEntry.hpp"
#include "GameState.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyFixture.hpp"

using rtype::network::ConnectionAccepted;
using rtype::network::ConnectionRefused;
using rtype::network::MessageType;
using rtype::network::RefusalReason;
using rtype::server::GameState;
using rtype::server::testing::holdsMessageOfType;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::sentTo;

/**
 * Given a game waiting for players
 * When a stranger asks for a place without a cookie
 * Then it gets a challenge, and nobody sits in the game
 */
TEST_F(ServerLobbyFixture, ChallengesAStrangerWhileTheGameWaits) {
  startWith(shippedWaves());

  runTick({}, {requestFrom(0)});

  EXPECT_EQ(datagramsTo(0, MessageType::ConnectionChallenge), 1U);
  EXPECT_TRUE(probe().runningGame()->game().roster().endpoints().empty());
}

/**
 * Given a stranger challenged by the server
 * When it asks again with the cookie of the challenge
 * Then it sits in the game and the network admits its address and token
 */
TEST_F(ServerLobbyFixture, SeatsAPlayerWhoAnswersTheChallenge) {
  startWith(shippedWaves());
  runTick({}, {requestFrom(0)});
  const std::uint64_t cookie = challengeCookieOf(0);

  runTick({}, {requestFrom(0, cookie)});

  EXPECT_EQ(probe().runningGame()->game().roster().endpoints(),
            (std::vector<rtype::network::Endpoint>{playerEndpoint(0)}));
  ASSERT_EQ(network().admission().admitted().size(), 1U);
  EXPECT_EQ(network().admission().admitted().front().player, playerEndpoint(0));
  EXPECT_EQ(datagramsTo(0, MessageType::ConnectionAccepted), 1U);
}

/**
 * Given a game that left the waiting room because its only player is ready
 * When a stranger asks for a place
 * Then it is refused with GameInProgress
 */
TEST_F(ServerLobbyFixture, RefusesANewPlayerWhileTheGameIsInProgress) {
  startWith(shippedWaves());
  join(0);
  sayReady(0);
  ASSERT_EQ(probe().runningGame()->game().state(), GameState::Starting);

  runTick({}, {requestFrom(1)});

  const auto received = sentTo(network().sender().sent(), playerEndpoint(1));
  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(received.front()).reason,
            RefusalReason::GameInProgress);
}

/**
 * Given a player seated through its challenge, then the game filled up
 * When the player asks again with the same cookie
 * Then it is not refused: the lobby answers it as a seated player, not as a
 * newcomer to a full game
 */
TEST_F(ServerLobbyFixture, AcceptsAgainASeatedPlayerOfAFullGame) {
  startWith(shippedWaves());
  runTick({}, {requestFrom(0)});
  const std::uint64_t cookie = challengeCookieOf(0);
  runTick({}, {requestFrom(0, cookie)});
  for (std::size_t index = 1; index < rtype::network::MAXIMUM_PLAYERS;
       ++index) {
    join(index);
  }

  runTick({}, {requestFrom(0, cookie)});

  EXPECT_EQ(datagramsTo(0, MessageType::ConnectionRefused), 0U);
}

/**
 * Given a server whose game plays the sheets the game ships with
 * When a player answers its challenge
 * Then its acceptance carries the five sheets the game is made from, in the
 * order of their names: the lobby hands the acceptor the table of the game
 */
TEST_F(ServerLobbyFixture, AcceptsWithTheEntityTypesOfTheGame) {
  startWith(shippedWaves());
  runTick({}, {requestFrom(0)});
  const std::uint64_t cookie = challengeCookieOf(0);

  runTick({}, {requestFrom(0, cookie)});

  const std::vector<rtype::network::OutgoingDatagram> sent =
      sentTo(network().sender().sent(), playerEndpoint(0));
  const auto acceptance = std::ranges::find_if(
      sent, [](const rtype::network::OutgoingDatagram& datagram) {
        return holdsMessageOfType(datagram, MessageType::ConnectionAccepted);
      });
  ASSERT_NE(acceptance, sent.end());
  std::vector<std::string> names;
  for (const rtype::network::EntityTypeEntry& entry :
       onlyMessageIn<ConnectionAccepted>(*acceptance).types) {
    names.push_back(entry.name);
  }
  EXPECT_EQ(names,
            (std::vector<std::string>{
                "bydo_tyrant", "bydomancer", "drifter", "drillback",
                "enemy_missile", "orb_shard", "player_missile", "reaper",
                "rune_orb", "ship", "tyrant_diver", "tyrant_missile",
                "tyrant_spore", "tyrant_spore_shard", "voltjelly", "weaver"}));
}
