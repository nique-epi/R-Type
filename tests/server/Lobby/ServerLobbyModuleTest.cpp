#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "AssetFolder.hpp"
#include "BanFixtures.hpp"
#include "BanKind.hpp"
#include "BanTestConstants.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionRefused.hpp"
#include "ContentAnnouncementReading.hpp"
#include "Endpoint.hpp"
#include "Expulsion.hpp"
#include "GameState.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyFixture.hpp"
#include "ServerLobbyTestConstants.hpp"

using rtype::network::ConnectionAccepted;
using rtype::network::ConnectionRefused;
using rtype::network::Expulsion;
using rtype::network::ExpulsionReason;
using rtype::network::MessageType;
using rtype::network::RefusalReason;
using rtype::server::BanKind;
using rtype::server::FAREWELL_COPIES;
using rtype::server::GameState;
using rtype::server::testing::BANNED_NICKNAME;
using rtype::server::testing::banOf;
using rtype::server::testing::GAME_PACK_PORT;
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
 * Given a game process whose supervisor serves on a known port, on the assets
 * next to it
 * When a player answers its challenge
 * Then its acceptance announces the pack of those assets, the one the
 * supervisor serves from the same folder, on that port
 */
TEST_F(ServerLobbyFixture, AcceptsWithTheContentOfItsAssets) {
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
  EXPECT_EQ(
      onlyMessageIn<ConnectionAccepted>(*acceptance).content,
      rtype::server::contentAnnouncementOf(
          std::array{rtype::engine::locateAssetFolder()}, GAME_PACK_PORT));
}

/**
 * Given a game with one seated pilot, ace
 * When the storage module publishes a ban for good of ACE and a tick runs
 * Then the pilot gets FAREWELL_COPIES Expulsion datagrams of the reason Banned
 */
TEST_F(ServerLobbyFixture, ExpelsASeatedPilotOnceItsBanIsPublished) {
  startWith(shippedWaves());
  join(0);

  publishBans({banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt)});
  runTick({});

  EXPECT_EQ(datagramsTo(0, MessageType::Expulsion), FAREWELL_COPIES);
  EXPECT_EQ(onlyMessageIn<Expulsion>(
                sentTo(network().sender().sent(), playerEndpoint(0)).back())
                .reason,
            ExpulsionReason::Banned);
}

/**
 * Given bans published and a tick run since
 * When a pilot the bans bar is seated without going through the lobby, then a
 * tick runs while no newer bans were published
 * Then that pilot gets no Expulsion: the bans are applied to the seated pilots
 * once per revision, and a newcomer is judged when it asks to join
 */
TEST_F(ServerLobbyFixture, ExpelsNobodyWhenTheBansDidNotChange) {
  startWith(shippedWaves());
  publishBans({banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt)});
  runTick({});
  join(1);

  runTick({});

  EXPECT_EQ(datagramsTo(1, MessageType::Expulsion), 0U);
}
