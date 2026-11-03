#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Heartbeat.hpp"
#include "Heartbeats.hpp"
#include "MessageOrigin.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTrips.hpp"
#include "SentDatagrams.hpp"
#include "SessionDatagram.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::HEARTBEAT_SENDING_TIME;
using testing::HEARTBEAT_SENDING_TIME_ON_THE_WIRE;
using testing::heartbeatsSentAt;
using testing::HELD_UNTIL_THE_SERVER_SENDS;
using testing::MEASURED_PLAYER_ROUND_TRIP;
using testing::MEASURED_PLAYER_ROUND_TRIP_ON_THE_WIRE;
using testing::onlyMessageIn;
using testing::onlyMessageTypeOf;
using testing::PLAYER_HEARTBEAT_ARRIVAL;
using testing::PLAYER_HEARTBEAT_SENT_AT;
using testing::playerEndpoint;
using testing::PROTOCOL_TIME_BETWEEN_HEARTBEATS;
using testing::readEchoFrom;
using testing::sentTo;
using testing::SOME_STALL;
using testing::SOME_TICK_TIME;

/**
 * Given two players, the server sent the first a Heartbeat in the previous
 * tick and the second one tick earlier
 * When a tick starts SOME_TICK_TIME after the previous one
 * Then the first was sent no Heartbeat for that time, and the second for twice
 * that time
 */
TEST(Heartbeats, AddsTheTimeOfTheTickToEveryPlayer) {
  PlayerRoster roster;
  const ConnectedPlayer& sentLastTick = addPlayer(roster, 0);
  ConnectedPlayer& sentTheTickBefore = addPlayer(roster, 1);
  sentTheTickBefore.sinceHeartbeat = SOME_TICK_TIME;

  addTimeSinceHeartbeat(roster, SOME_TICK_TIME);

  EXPECT_EQ(sentLastTick.sinceHeartbeat, SOME_TICK_TIME);
  EXPECT_EQ(sentTheTickBefore.sinceHeartbeat, 2 * SOME_TICK_TIME);
}

/**
 * Given a player the server sent no Heartbeat for one nanosecond less than
 * the whole ticks that fit in the interval of the protocol
 * When the heartbeats are sent
 * Then nothing goes out
 */
TEST(Heartbeats, SendsNothingBeforeTheInterval) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.sinceHeartbeat = PROTOCOL_TIME_BETWEEN_HEARTBEATS - ONE_NANOSECOND;

  const std::vector<network::OutgoingDatagram> sent =
      heartbeatsSentAt(roster, HEARTBEAT_SENDING_TIME);

  EXPECT_TRUE(sent.empty());
}

/**
 * Given a player the server sent no Heartbeat for the whole ticks that fit in
 * the interval of the protocol
 * When the heartbeats are sent
 * Then the player is sent a Heartbeat, then the round trips, and its time
 * since a Heartbeat starts again from zero
 */
TEST(Heartbeats, SendsAHeartbeatThenTheRoundTripsAtTheInterval) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.sinceHeartbeat = PROTOCOL_TIME_BETWEEN_HEARTBEATS;

  const std::vector<network::OutgoingDatagram> sent =
      heartbeatsSentAt(roster, HEARTBEAT_SENDING_TIME);

  ASSERT_EQ(sent.size(), 2U);
  EXPECT_EQ(sent.front().destination, player.endpoint);
  EXPECT_EQ(onlyMessageTypeOf(sent.front()), network::MessageType::Heartbeat);
  EXPECT_EQ(sent.back().destination, player.endpoint);
  EXPECT_EQ(onlyMessageTypeOf(sent.back()), network::MessageType::RoundTrips);
  EXPECT_EQ(player.sinceHeartbeat, engine::Duration::zero());
}

/**
 * Given a player the server sent no Heartbeat for the interval
 * When the header of another datagram to the player is taken, then the
 * heartbeats are sent
 * Then the player is still sent a Heartbeat: other datagrams do not delay it
 */
TEST(Heartbeats, AnotherDatagramDoesNotDelayTheHeartbeat) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.sinceHeartbeat = PROTOCOL_TIME_BETWEEN_HEARTBEATS;

  static_cast<void>(nextSessionHeader(player));
  const std::vector<network::OutgoingDatagram> sent =
      heartbeatsSentAt(roster, HEARTBEAT_SENDING_TIME);

  ASSERT_FALSE(sent.empty());
  EXPECT_EQ(onlyMessageTypeOf(sent.front()), network::MessageType::Heartbeat);
}

/**
 * Given a player due a Heartbeat
 * When the heartbeats are sent at 3 s
 * Then the Heartbeat carries 3000000 µs, and no echo before the player sent
 * one
 */
TEST(Heartbeats, TheHeartbeatCarriesTheTimeItIsSent) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.sinceHeartbeat = PROTOCOL_TIME_BETWEEN_HEARTBEATS;

  const std::vector<network::OutgoingDatagram> sent =
      heartbeatsSentAt(roster, HEARTBEAT_SENDING_TIME);

  ASSERT_FALSE(sent.empty());
  EXPECT_EQ(onlyMessageIn<network::Heartbeat>(sent.front()),
            (network::Heartbeat{.sentAt = HEARTBEAT_SENDING_TIME_ON_THE_WIRE,
                                .echo = std::nullopt}));
}

/**
 * Given a player whose Heartbeat sent at 9 µs arrived 100 ms before the
 * server sends its own
 * When the heartbeats are sent
 * Then the Heartbeat to the player echoes it, held 100000 µs
 */
TEST(Heartbeats, TheHeartbeatEchoesTheNewestHeartbeatOfThePlayer) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  readHeartbeat(roster,
                network::MessageOrigin{.sender = player.endpoint,
                                       .header = {},
                                       .receivedAt = PLAYER_HEARTBEAT_ARRIVAL},
                network::Heartbeat{.sentAt = PLAYER_HEARTBEAT_SENT_AT,
                                   .echo = std::nullopt});
  player.sinceHeartbeat = PROTOCOL_TIME_BETWEEN_HEARTBEATS;

  const std::vector<network::OutgoingDatagram> sent =
      heartbeatsSentAt(roster, HEARTBEAT_SENDING_TIME);

  ASSERT_FALSE(sent.empty());
  EXPECT_EQ(onlyMessageIn<network::Heartbeat>(sent.front()).echo,
            (network::HeartbeatEcho{.sentAt = PLAYER_HEARTBEAT_SENT_AT,
                                    .heldFor = HELD_UNTIL_THE_SERVER_SENDS}));
}

/**
 * Given a player the server sent a Heartbeat at 3 s
 * When the player echoes it, held 2 ms, and the echo arrives 50 ms after the
 * Heartbeat left
 * Then the round trip of the player is 48 ms
 */
TEST(Heartbeats, ReadsTheRoundTripOfThePlayerFromItsEcho) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);

  readEchoFrom(roster, player.endpoint);

  const std::optional<network::RoundTripFigures> figures =
      player.roundTrip.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(network::RoundTripFigures{}).roundTrip,
            MEASURED_PLAYER_ROUND_TRIP);
}

/**
 * Given a roster with one player
 * When a Heartbeat with an echo comes from an address the roster does not hold
 * Then nothing changes and nothing throws: no player was measured
 */
TEST(Heartbeats, IgnoresAHeartbeatFromAStranger) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);

  readEchoFrom(roster, playerEndpoint(1));

  EXPECT_FALSE(player.roundTrip.figures().has_value());
}

/**
 * Given two players, only the first of them measured, both due a Heartbeat
 * When the heartbeats are sent
 * Then the round trips sent list the first player alone, with its round trip
 */
TEST(Heartbeats, TheRoundTripsListOnlyThePlayersMeasured) {
  PlayerRoster roster;
  ConnectedPlayer& measured = addPlayer(roster, 0);
  ConnectedPlayer& unmeasured = addPlayer(roster, 1);
  readEchoFrom(roster, measured.endpoint);
  measured.sinceHeartbeat = PROTOCOL_TIME_BETWEEN_HEARTBEATS;
  unmeasured.sinceHeartbeat = PROTOCOL_TIME_BETWEEN_HEARTBEATS;

  const std::vector<network::OutgoingDatagram> sent =
      heartbeatsSentAt(roster, HEARTBEAT_SENDING_TIME);

  const std::vector<network::OutgoingDatagram> toUnmeasured =
      sentTo(sent, unmeasured.endpoint);
  ASSERT_EQ(toUnmeasured.size(), 2U);
  const auto report = onlyMessageIn<network::RoundTrips>(toUnmeasured.back());
  EXPECT_EQ(report.players,
            (std::vector<network::PlayerRoundTrip>{
                {.playerNumber = measured.playerNumber,
                 .roundTrip = MEASURED_PLAYER_ROUND_TRIP_ON_THE_WIRE,
                 .jitter = 0}}));
}

/**
 * Given two players the server sent a Heartbeat just before it stalled
 * When a tick starts SOME_STALL after the previous one, then the heartbeats
 * are sent
 * Then each player is sent a Heartbeat and the round trips: its client
 * counted the stall too
 */
TEST(Heartbeats, SendsEveryPlayerAHeartbeatAfterAStall) {
  PlayerRoster roster;
  const ConnectedPlayer& first = addPlayer(roster, 0);
  const ConnectedPlayer& second = addPlayer(roster, 1);

  addTimeSinceHeartbeat(roster, SOME_STALL);
  const std::vector<network::OutgoingDatagram> sent =
      heartbeatsSentAt(roster, HEARTBEAT_SENDING_TIME);

  EXPECT_EQ(sentTo(sent, first.endpoint).size(), 2U);
  EXPECT_EQ(sentTo(sent, second.endpoint).size(), 2U);
}

}  // namespace rtype::server
