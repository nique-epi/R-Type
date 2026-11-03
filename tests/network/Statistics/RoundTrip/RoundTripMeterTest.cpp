#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include "Heartbeat.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripMeter.hpp"
#include "RoundTripMeterTestConstants.hpp"

namespace rtype::network {

using testing::ECHO_BACK_AFTER;
using testing::FIRST_SENDING_TIME;
using testing::FIRST_SENDING_TIME_ON_THE_WIRE;
using testing::HELD_LONGER_THAN_THE_TRIP;
using testing::HELD_THE_LONGEST_ROUND_TRIP;
using testing::LATE_ARRIVAL;
using testing::MEASURED_ROUND_TRIP;
using testing::MEASURED_SENDING_TIME;
using testing::NEWER_PEER_ARRIVAL;
using testing::NEWER_PEER_SENT_AT;
using testing::OLDER_PEER_ARRIVAL;
using testing::OLDER_PEER_SENT_AT;
using testing::ONE_MILLISECOND_BEFORE_THE_WRAP;
using testing::PEER_ARRIVAL;
using testing::SENDING_AFTER_THE_NEWER;
using testing::SENDING_PAST_THE_LONGEST_ROUND_TRIP_AFTER;
using testing::SENDING_THE_LONGEST_ROUND_TRIP_AFTER;
using testing::SENT_AT_TWENTY_FIVE_SECONDS_EARLIER;
using testing::THREE_MILLISECONDS_HELD;

/**
 * Given a meter that received no Heartbeat
 * When it builds a Heartbeat at 1 s
 * Then the Heartbeat carries 1000000 µs and no echo
 */
TEST(RoundTripMeter, SendsNoEchoBeforeAnyHeartbeat) {
  const RoundTripMeter meter;

  const Heartbeat heartbeat = meter.nextHeartbeat(FIRST_SENDING_TIME);

  EXPECT_EQ(heartbeat, (Heartbeat{.sentAt = FIRST_SENDING_TIME_ON_THE_WIRE,
                                  .echo = std::nullopt}));
}

/**
 * Given a meter that received a Heartbeat sent at 7 µs at 2 s, then one sent
 * at 9 µs at 2.5 s
 * When it builds a Heartbeat 3 ms after the second arrived
 * Then the Heartbeat echoes the second, held 3000 µs
 */
TEST(RoundTripMeter, EchoesTheNewestHeartbeatWithTheTimeHeld) {
  RoundTripMeter meter;
  meter.receive(Heartbeat{.sentAt = OLDER_PEER_SENT_AT, .echo = std::nullopt},
                OLDER_PEER_ARRIVAL);
  meter.receive(Heartbeat{.sentAt = NEWER_PEER_SENT_AT, .echo = std::nullopt},
                NEWER_PEER_ARRIVAL);

  const Heartbeat heartbeat = meter.nextHeartbeat(SENDING_AFTER_THE_NEWER);

  EXPECT_EQ(heartbeat.echo,
            (HeartbeatEcho{.sentAt = NEWER_PEER_SENT_AT,
                           .heldFor = THREE_MILLISECONDS_HELD}));
}

/**
 * Given a meter that sent a Heartbeat at 10 s
 * When the echo of that Heartbeat, held 3 ms by the peer, arrives 45 ms after
 * it left
 * Then the meter measured one round trip of 42 ms, with a jitter of 0
 */
TEST(RoundTripMeter, MeasuresTheRoundTripLessTheTimeHeld) {
  RoundTripMeter meter;
  const Heartbeat sent = meter.nextHeartbeat(MEASURED_SENDING_TIME);

  meter.receive(
      Heartbeat{.sentAt = NEWER_PEER_SENT_AT,
                .echo = HeartbeatEcho{.sentAt = sent.sentAt,
                                      .heldFor = THREE_MILLISECONDS_HELD}},
      MEASURED_SENDING_TIME + ECHO_BACK_AFTER);

  const std::optional<RoundTripFigures> figures = meter.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}),
            (RoundTripFigures{.roundTrip = MEASURED_ROUND_TRIP,
                              .jitter = std::chrono::microseconds::zero()}));
}

/**
 * Given a meter that sent a Heartbeat 1 ms before its microsecond counter
 * wraps to 0
 * When its echo, held 3 ms, arrives 45 ms after it left, its counter past 0
 * Then the meter still measured a round trip of 42 ms
 */
TEST(RoundTripMeter, MeasuresAcrossTheWrapOfTheMicrosecondCounter) {
  RoundTripMeter meter;
  const Heartbeat sent = meter.nextHeartbeat(ONE_MILLISECOND_BEFORE_THE_WRAP);

  meter.receive(
      Heartbeat{.sentAt = NEWER_PEER_SENT_AT,
                .echo = HeartbeatEcho{.sentAt = sent.sentAt,
                                      .heldFor = THREE_MILLISECONDS_HELD}},
      ONE_MILLISECOND_BEFORE_THE_WRAP + ECHO_BACK_AFTER);

  const std::optional<RoundTripFigures> figures = meter.figures();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(RoundTripFigures{}).roundTrip,
            MEASURED_ROUND_TRIP);
}

/**
 * Given a meter that sent a Heartbeat at 10 s
 * When an echo of it held 50 ms arrives 45 ms after it left
 * Then the meter measured nothing: no peer holds a Heartbeat longer than it
 * travelled
 */
TEST(RoundTripMeter, IgnoresAnEchoHeldLongerThanSinceItWasSent) {
  RoundTripMeter meter;
  const Heartbeat sent = meter.nextHeartbeat(MEASURED_SENDING_TIME);

  meter.receive(
      Heartbeat{.sentAt = NEWER_PEER_SENT_AT,
                .echo = HeartbeatEcho{.sentAt = sent.sentAt,
                                      .heldFor = HELD_LONGER_THAN_THE_TRIP}},
      MEASURED_SENDING_TIME + ECHO_BACK_AFTER);

  EXPECT_FALSE(meter.figures().has_value());
}

/**
 * Given a meter
 * When an echo of a time 25 s before its arrival comes, held for nothing
 * Then the meter measured nothing: a session lives at most 20 s without a
 * datagram, so this side never sent that time
 */
TEST(RoundTripMeter, IgnoresAnEchoOfATimeNeverSent) {
  RoundTripMeter meter;

  meter.receive(
      Heartbeat{
          .sentAt = NEWER_PEER_SENT_AT,
          .echo = HeartbeatEcho{.sentAt = SENT_AT_TWENTY_FIVE_SECONDS_EARLIER,
                                .heldFor = 0}},
      LATE_ARRIVAL);

  EXPECT_FALSE(meter.figures().has_value());
}

/**
 * Given a meter that received a Heartbeat at 1 s
 * When it builds a Heartbeat 20 s and 1 µs later
 * Then the Heartbeat carries no echo
 */
TEST(RoundTripMeter, StopsEchoingAHeartbeatPastTheLongestRoundTrip) {
  RoundTripMeter meter;
  meter.receive(Heartbeat{.sentAt = NEWER_PEER_SENT_AT, .echo = std::nullopt},
                PEER_ARRIVAL);

  const Heartbeat heartbeat =
      meter.nextHeartbeat(SENDING_PAST_THE_LONGEST_ROUND_TRIP_AFTER);

  EXPECT_FALSE(heartbeat.echo.has_value());
}

/**
 * Given a meter that received a Heartbeat at 1 s
 * When it builds a Heartbeat exactly 20 s later
 * Then the Heartbeat still echoes it, held 20 s
 */
TEST(RoundTripMeter, EchoesAHeartbeatHeldExactlyTheLongestRoundTrip) {
  RoundTripMeter meter;
  meter.receive(Heartbeat{.sentAt = NEWER_PEER_SENT_AT, .echo = std::nullopt},
                PEER_ARRIVAL);

  const Heartbeat heartbeat =
      meter.nextHeartbeat(SENDING_THE_LONGEST_ROUND_TRIP_AFTER);

  EXPECT_EQ(heartbeat.echo,
            (HeartbeatEcho{.sentAt = NEWER_PEER_SENT_AT,
                           .heldFor = HELD_THE_LONGEST_ROUND_TRIP}));
}

}  // namespace rtype::network
