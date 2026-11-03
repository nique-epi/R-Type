#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "Heartbeat.hpp"
#include "MessageType.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripWatchFixture.hpp"
#include "RoundTripWatchTestConstants.hpp"
#include "RoundTrips.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::client {

using testing::CLOCK_WHEN_THE_SESSION_OPENS;
using testing::ECHO_ARRIVAL;
using testing::FIRST_HEARTBEAT_SENT_AT;
using testing::HELD_BY_THE_SERVER;
using testing::HELD_UNTIL_THE_CLIENT_SENDS;
using testing::OTHER_PLAYER_NUMBER;
using testing::OWN_PLAYER_NUMBER;
using testing::OWN_ROUND_TRIP;
using testing::PROTOCOL_HEARTBEAT_INTERVAL;
using testing::REPORTED_JITTER;
using testing::REPORTED_JITTER_DURATION;
using testing::REPORTED_ROUND_TRIP;
using testing::REPORTED_ROUND_TRIP_DURATION;
using testing::RoundTripWatchFixture;
using testing::SERVER_HEARTBEAT_ARRIVAL;
using testing::SERVER_HEARTBEAT_SENT_AT;
using testing::UNREPORTED_PLAYER_NUMBER;

/**
 * Given a watch without a session
 * When the interval of the protocol passes
 * Then no Heartbeat is sent
 */
TEST_F(RoundTripWatchFixture, SendsNothingWithoutASession) {
  hold(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(outbox().sentCountOf(network::MessageType::Heartbeat), 0U);
}

/**
 * Given a session that just opened
 * When the interval less one nanosecond passes, then one more nanosecond
 * Then no Heartbeat is sent before the interval, and one at it
 */
TEST_F(RoundTripWatchFixture, WaitsAnIntervalBeforeTheFirstHeartbeat) {
  openSession();

  hold(PROTOCOL_HEARTBEAT_INTERVAL - ONE_NANOSECOND);
  const std::size_t sentBeforeTheInterval =
      outbox().sentCountOf(network::MessageType::Heartbeat);
  hold(ONE_NANOSECOND);

  EXPECT_EQ(sentBeforeTheInterval, 0U);
  EXPECT_EQ(outbox().sentCountOf(network::MessageType::Heartbeat), 1U);
}

/**
 * Given a session that just opened
 * When three intervals pass, one per frame
 * Then a Heartbeat goes out after each of them
 */
TEST_F(RoundTripWatchFixture, SendsAHeartbeatEveryInterval) {
  openSession();

  hold(PROTOCOL_HEARTBEAT_INTERVAL);
  hold(PROTOCOL_HEARTBEAT_INTERVAL);
  hold(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(outbox().sentCountOf(network::MessageType::Heartbeat), 3U);
}

/**
 * Given a session that opened at 3.5 s on the clock
 * When the interval passes, to 4 s
 * Then the Heartbeat sent carries 4000000 µs, and no echo before the server
 * sent one
 */
TEST_F(RoundTripWatchFixture, TheHeartbeatCarriesTheTimeOnTheClock) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();

  hold(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(outbox().lastSent<network::Heartbeat>(),
            (network::Heartbeat{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                .echo = std::nullopt}));
}

/**
 * Given a session that opened at 3.5 s, and a Heartbeat of the server sent at
 * 9 µs that arrived at 3.99 s
 * When the interval passes, to 4 s
 * Then the Heartbeat sent echoes the one of the server, held 10000 µs
 */
TEST_F(RoundTripWatchFixture, EchoesTheNewestHeartbeatOfTheServer) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  deliver(network::Heartbeat{.sentAt = SERVER_HEARTBEAT_SENT_AT,
                             .echo = std::nullopt},
          SERVER_HEARTBEAT_ARRIVAL);

  hold(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(outbox().lastSent<network::Heartbeat>().echo,
            (network::HeartbeatEcho{.sentAt = SERVER_HEARTBEAT_SENT_AT,
                                    .heldFor = HELD_UNTIL_THE_CLIENT_SENDS}));
}

/**
 * Given a session that sent its first Heartbeat at 4 s
 * When the echo of it, held 2 ms by the server, arrives at 4.05 s
 * Then the round trip of the own player is 48 ms
 */
TEST_F(RoundTripWatchFixture, MeasuresItsOwnRoundTripFromTheEcho) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(PROTOCOL_HEARTBEAT_INTERVAL);

  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER}},
      ECHO_ARRIVAL);

  const std::optional<network::RoundTripFigures> figures =
      watch().roundTripOf(OWN_PLAYER_NUMBER);
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(network::RoundTripFigures{}).roundTrip,
            OWN_ROUND_TRIP);
}

/**
 * Given a session that measured nothing yet
 * When the round trip of the own player is asked
 * Then there is none
 */
TEST_F(RoundTripWatchFixture, GivesNothingForItsOwnPlayerBeforeAnEcho) {
  openSession();

  EXPECT_FALSE(watch().roundTripOf(OWN_PLAYER_NUMBER).has_value());
}

/**
 * Given a session
 * When the server reports a round trip of 80 ms and a jitter of 12.5 ms for
 * another player
 * Then that player has those figures
 */
TEST_F(RoundTripWatchFixture, GivesTheRoundTripsTheServerReportedForTheOthers) {
  openSession();

  deliver(network::RoundTrips{.players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                                           .roundTrip = REPORTED_ROUND_TRIP,
                                           .jitter = REPORTED_JITTER}}},
          SERVER_HEARTBEAT_ARRIVAL);

  EXPECT_EQ(
      watch().roundTripOf(OTHER_PLAYER_NUMBER),
      (network::RoundTripFigures{.roundTrip = REPORTED_ROUND_TRIP_DURATION,
                                 .jitter = REPORTED_JITTER_DURATION}));
}

/**
 * Given a session whose server reported another player
 * When the round trip of a player it did not report is asked
 * Then there is none
 */
TEST_F(RoundTripWatchFixture, GivesNothingForAPlayerNotReported) {
  openSession();
  deliver(network::RoundTrips{.players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                                           .roundTrip = REPORTED_ROUND_TRIP,
                                           .jitter = REPORTED_JITTER}}},
          SERVER_HEARTBEAT_ARRIVAL);

  EXPECT_FALSE(watch().roundTripOf(UNREPORTED_PLAYER_NUMBER).has_value());
}

/**
 * Given a session whose server reported another player
 * When the session closes, then an interval passes
 * Then no player has a round trip any more, and no Heartbeat is sent
 */
TEST_F(RoundTripWatchFixture, ForgetsEverythingWhenTheSessionCloses) {
  openSession();
  deliver(network::RoundTrips{.players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                                           .roundTrip = REPORTED_ROUND_TRIP,
                                           .jitter = REPORTED_JITTER}}},
          SERVER_HEARTBEAT_ARRIVAL);

  watch().sessionClosed();
  hold(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_FALSE(watch().roundTripOf(OTHER_PLAYER_NUMBER).has_value());
  EXPECT_EQ(outbox().sentCountOf(network::MessageType::Heartbeat), 0U);
}

/**
 * Given a watch without a session, which the server's round trips reach
 * When a session opens
 * Then the round trips that came before it are not kept
 */
TEST_F(RoundTripWatchFixture, IgnoresAReportWithoutASession) {
  deliver(network::RoundTrips{.players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                                           .roundTrip = REPORTED_ROUND_TRIP,
                                           .jitter = REPORTED_JITTER}}},
          SERVER_HEARTBEAT_ARRIVAL);

  openSession();

  EXPECT_FALSE(watch().roundTripOf(OTHER_PLAYER_NUMBER).has_value());
}

}  // namespace rtype::client
