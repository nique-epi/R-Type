#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "Heartbeat.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripSample.hpp"
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
using testing::ONE_FRAME;
using testing::ONE_HEARTBEAT_INTERVAL;
using testing::OWN_PLAYER_NUMBER;
using testing::OWN_ROUND_TRIP;
using testing::REPORTED_JITTER;
using testing::REPORTED_JITTER_DURATION;
using testing::REPORTED_PLAYER_NUMBER;
using testing::REPORTED_ROUND_TRIP;
using testing::REPORTED_ROUND_TRIP_DURATION;
using testing::RoundTripWatchFixture;
using testing::SECOND_ECHO_ARRIVAL;
using testing::SECOND_HEARTBEAT_SENT_AT;
using testing::SECOND_OWN_ROUND_TRIP;
using testing::SECOND_SAMPLE_NUMBER;
using testing::SENT_AT_A_FRAME_AFTER_THE_OPENING;
using testing::SERVER_HEARTBEAT_ARRIVAL;
using testing::SERVER_HEARTBEAT_SENT_AT;
using testing::SERVER_HEARTBEAT_TICK;
using testing::SERVER_HEARTBEAT_TICK_PLACED_AT;
using testing::SERVER_TICK_TOLERANCE;
using testing::TICK_AT_THE_PLACED_INSTANT;
using testing::TO_REACH_THE_PLACED_TICK;
using testing::UNREPORTED_PLAYER_NUMBER;

/**
 * Given a watch without a session
 * When the interval of the protocol passes
 * Then no Heartbeat is sent
 */
TEST_F(RoundTripWatchFixture, SendsNothingWithoutASession) {
  hold(ONE_HEARTBEAT_INTERVAL);

  EXPECT_EQ(heartbeatsSent(), 0U);
}

/**
 * Given a session that just opened
 * When the interval less one nanosecond passes, then one more nanosecond
 * Then no Heartbeat is sent before the interval, and one at it
 */
TEST_F(RoundTripWatchFixture, WaitsAnIntervalBeforeTheFirstHeartbeat) {
  openSession();

  hold(ONE_HEARTBEAT_INTERVAL - ONE_NANOSECOND);
  const std::size_t sentBeforeTheInterval = heartbeatsSent();
  hold(ONE_NANOSECOND);

  EXPECT_EQ(sentBeforeTheInterval, 0U);
  EXPECT_EQ(heartbeatsSent(), 1U);
}

/**
 * Given a session that just opened
 * When three intervals pass, one per frame
 * Then a Heartbeat goes out after each of them
 */
TEST_F(RoundTripWatchFixture, SendsAHeartbeatEveryInterval) {
  openSession();

  hold(ONE_HEARTBEAT_INTERVAL);
  hold(ONE_HEARTBEAT_INTERVAL);
  hold(ONE_HEARTBEAT_INTERVAL);

  EXPECT_EQ(heartbeatsSent(), 3U);
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

  hold(ONE_HEARTBEAT_INTERVAL);

  EXPECT_EQ(lastSentHeartbeat(),
            (network::Heartbeat{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                .echo = std::nullopt}));
}

/**
 * Given a session that opened at 3.5 s, where the outbox owes the server the
 * acknowledgement of guaranteed messages
 * When a frame of 1 ms goes by, far short of the interval
 * Then a Heartbeat goes out at once, carrying 3501000 µs: the acknowledgement
 * of the guaranteed messages also measures the round trip
 */
TEST_F(RoundTripWatchFixture, SendsATimedHeartbeatOnceAnAcknowledgementIsDue) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  oweAnAcknowledgement();

  hold(ONE_FRAME);

  ASSERT_EQ(heartbeatsSent(), 1U);
  EXPECT_EQ(lastSentHeartbeat().sentAt, SENT_AT_A_FRAME_AFTER_THE_OPENING);
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

  hold(ONE_HEARTBEAT_INTERVAL);

  EXPECT_EQ(lastSentHeartbeat().echo,
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
  hold(ONE_HEARTBEAT_INTERVAL);

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
 * Given a session that sent its first Heartbeat at 4 s
 * When the echo of it, held 2 ms, arrives at 4.05 s in a Heartbeat of the
 * server at tick 600
 * Then the clock places tick 600 half of the round trip of 48 ms before the
 * datagram arrived, at 4.026 s
 */
TEST_F(RoundTripWatchFixture,
       PlacesTheTickOfTheServerHalfARoundTripBeforeItsArrival) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(ONE_HEARTBEAT_INTERVAL);

  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER},
          .tick = SERVER_HEARTBEAT_TICK},
      ECHO_ARRIVAL);

  EXPECT_EQ(watch().serverClock().localInstantOf(SERVER_HEARTBEAT_TICK),
            std::optional<engine::Duration>{
                engine::Duration{SERVER_HEARTBEAT_TICK_PLACED_AT}});
}

/**
 * Given a session whose clock placed tick 600 at 4.026 s
 * When the clock of the client reaches 4.026 s
 * Then the server is at tick 600
 */
TEST_F(RoundTripWatchFixture, ReadsTheTickOfTheServerOnTheClockOfTheClient) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(ONE_HEARTBEAT_INTERVAL);
  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER},
          .tick = SERVER_HEARTBEAT_TICK},
      ECHO_ARRIVAL);

  advanceClock(TO_REACH_THE_PLACED_TICK);

  EXPECT_NEAR(watch().serverTickNow().value_or(0.0), TICK_AT_THE_PLACED_INSTANT,
              SERVER_TICK_TOLERANCE);
}

/**
 * Given a session that placed no tick of the server
 * When the tick of the server is asked
 * Then there is none
 */
TEST_F(RoundTripWatchFixture, KnowsNoTickOfTheServerBeforeAHeartbeatPlacedOne) {
  openSession();

  EXPECT_FALSE(watch().serverTickNow().has_value());
}

/**
 * Given a session in the waiting room
 * When a Heartbeat of the server that echoes ours but carries no tick arrives
 * Then the clock stays empty: no game runs, so no tick places any instant
 */
TEST_F(RoundTripWatchFixture, LeavesTheClockEmptyForAHeartbeatWithoutATick) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(ONE_HEARTBEAT_INTERVAL);

  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER},
          .tick = std::nullopt},
      ECHO_ARRIVAL);

  EXPECT_FALSE(
      watch().serverClock().localInstantOf(SERVER_HEARTBEAT_TICK).has_value());
}

/**
 * Given a session that sent a Heartbeat
 * When a Heartbeat of the server at tick 600 arrives with no echo, so no
 * round trip to halve
 * Then the clock stays empty: without it the trip of the datagram is unknown
 */
TEST_F(RoundTripWatchFixture, LeavesTheClockEmptyForAHeartbeatWithoutAnEcho) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(ONE_HEARTBEAT_INTERVAL);

  deliver(network::Heartbeat{.sentAt = SERVER_HEARTBEAT_SENT_AT,
                             .echo = std::nullopt,
                             .tick = SERVER_HEARTBEAT_TICK},
          ECHO_ARRIVAL);

  EXPECT_FALSE(
      watch().serverClock().localInstantOf(SERVER_HEARTBEAT_TICK).has_value());
}

/**
 * Given a session whose clock placed a tick of the server
 * When the session closes
 * Then the clock is empty again
 */
TEST_F(RoundTripWatchFixture, ForgetsTheClockWhenTheSessionCloses) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(ONE_HEARTBEAT_INTERVAL);
  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER},
          .tick = SERVER_HEARTBEAT_TICK},
      ECHO_ARRIVAL);

  watch().sessionClosed();

  EXPECT_FALSE(
      watch().serverClock().localInstantOf(SERVER_HEARTBEAT_TICK).has_value());
}

/**
 * Given no session open
 * When a Heartbeat of the server at tick 600 that closes a round trip arrives
 * Then the clock stays empty: what arrives without a session is ignored
 */
TEST_F(RoundTripWatchFixture, IgnoresTheTickOfAHeartbeatWithoutASession) {
  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER},
          .tick = SERVER_HEARTBEAT_TICK},
      ECHO_ARRIVAL);

  EXPECT_FALSE(
      watch().serverClock().localInstantOf(SERVER_HEARTBEAT_TICK).has_value());
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

  deliver(
      network::RoundTrips{.players = {{.playerNumber = REPORTED_PLAYER_NUMBER,
                                       .roundTrip = REPORTED_ROUND_TRIP,
                                       .jitter = REPORTED_JITTER}}},
      SERVER_HEARTBEAT_ARRIVAL);

  EXPECT_EQ(
      watch().roundTripOf(REPORTED_PLAYER_NUMBER),
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
  deliver(
      network::RoundTrips{.players = {{.playerNumber = REPORTED_PLAYER_NUMBER,
                                       .roundTrip = REPORTED_ROUND_TRIP,
                                       .jitter = REPORTED_JITTER}}},
      SERVER_HEARTBEAT_ARRIVAL);

  EXPECT_FALSE(watch().roundTripOf(UNREPORTED_PLAYER_NUMBER).has_value());
}

/**
 * Given a session whose server reported another player
 * When the session closes
 * Then no player has a round trip any more
 */
TEST_F(RoundTripWatchFixture,
       ForgetsTheReportedRoundTripsWhenTheSessionCloses) {
  openSession();
  deliver(
      network::RoundTrips{.players = {{.playerNumber = REPORTED_PLAYER_NUMBER,
                                       .roundTrip = REPORTED_ROUND_TRIP,
                                       .jitter = REPORTED_JITTER}}},
      SERVER_HEARTBEAT_ARRIVAL);

  watch().sessionClosed();

  EXPECT_FALSE(watch().roundTripOf(REPORTED_PLAYER_NUMBER).has_value());
}

/**
 * Given a session that closed
 * When an interval passes
 * Then no Heartbeat is sent
 */
TEST_F(RoundTripWatchFixture, SendsNothingOnceTheSessionClosed) {
  openSession();

  watch().sessionClosed();
  hold(ONE_HEARTBEAT_INTERVAL);

  EXPECT_EQ(heartbeatsSent(), 0U);
}

/**
 * Given a session that measured a round trip of 48 ms, closed, then a new one
 * opened
 * When the round trip of the own player is asked
 * Then there is none: the figures of the session before are not the new
 * server's
 */
TEST_F(RoundTripWatchFixture, ForgetsItsOwnRoundTripWhenTheSessionCloses) {
  reopenAfterAMeasuredRoundTrip();

  EXPECT_FALSE(watch().roundTripOf(OWN_PLAYER_NUMBER).has_value());
}

/**
 * Given a session that received a Heartbeat of its server, closed, then a new
 * one opened
 * When the interval passes
 * Then the Heartbeat sent echoes nothing: the Heartbeat kept came from the
 * clock of the server before
 */
TEST_F(RoundTripWatchFixture, EchoesNothingOfTheSessionBefore) {
  reopenAfterAMeasuredRoundTrip();

  hold(ONE_HEARTBEAT_INTERVAL);

  EXPECT_EQ(lastSentHeartbeat().echo, std::nullopt);
}

/**
 * Given a session whose server reported another player, then closed
 * When a new session opens
 * Then the round trips of the session before are not kept
 */
TEST_F(RoundTripWatchFixture, ForgetsTheReportOfTheSessionBefore) {
  openSession();
  deliver(
      network::RoundTrips{.players = {{.playerNumber = REPORTED_PLAYER_NUMBER,
                                       .roundTrip = REPORTED_ROUND_TRIP,
                                       .jitter = REPORTED_JITTER}}},
      SERVER_HEARTBEAT_ARRIVAL);
  watch().sessionClosed();

  openSession();

  EXPECT_FALSE(watch().roundTripOf(REPORTED_PLAYER_NUMBER).has_value());
}

/**
 * Given a watch without a session, which the server's round trips reach
 * When a session opens
 * Then the round trips that came before it are not kept
 */
TEST_F(RoundTripWatchFixture, IgnoresAReportWithoutASession) {
  deliver(
      network::RoundTrips{.players = {{.playerNumber = REPORTED_PLAYER_NUMBER,
                                       .roundTrip = REPORTED_ROUND_TRIP,
                                       .jitter = REPORTED_JITTER}}},
      SERVER_HEARTBEAT_ARRIVAL);

  openSession();

  EXPECT_FALSE(watch().roundTripOf(REPORTED_PLAYER_NUMBER).has_value());
}

/**
 * Given a session that measured a round trip of 48 ms
 * When the echo of its second Heartbeat gives a round trip of 128 ms
 * Then the newest round trip is the second one, 128 ms as measured, not the
 * smoothed 58 ms
 */
TEST_F(RoundTripWatchFixture, KeepsTheNewestRoundTripAsMeasured) {
  advanceClock(CLOCK_WHEN_THE_SESSION_OPENS);
  openSession();
  hold(ONE_HEARTBEAT_INTERVAL);
  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = FIRST_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER}},
      ECHO_ARRIVAL);
  hold(ONE_HEARTBEAT_INTERVAL);

  deliver(
      network::Heartbeat{
          .sentAt = SERVER_HEARTBEAT_SENT_AT,
          .echo = network::HeartbeatEcho{.sentAt = SECOND_HEARTBEAT_SENT_AT,
                                         .heldFor = HELD_BY_THE_SERVER}},
      SECOND_ECHO_ARRIVAL);

  EXPECT_EQ(watch().newestOwnRoundTrip(),
            (RoundTripSample{.number = SECOND_SAMPLE_NUMBER,
                             .roundTrip = SECOND_OWN_ROUND_TRIP}));
}

/**
 * Given a session that measured nothing yet
 * When the newest round trip is asked
 * Then there is none
 */
TEST_F(RoundTripWatchFixture, KeepsNoRoundTripBeforeAnEcho) {
  openSession();

  EXPECT_FALSE(watch().newestOwnRoundTrip().has_value());
}

/**
 * Given a session that measured a round trip, closed, then a new one opened
 * When the newest round trip is asked
 * Then there is none: it was measured with the server before
 */
TEST_F(RoundTripWatchFixture, ForgetsTheNewestRoundTripWhenTheSessionCloses) {
  reopenAfterAMeasuredRoundTrip();

  EXPECT_FALSE(watch().newestOwnRoundTrip().has_value());
}

}  // namespace rtype::client
