#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include "RoundTripFigures.hpp"
#include "RoundTripWire.hpp"
#include "RoundTripWireTestConstants.hpp"
#include "RoundTrips.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationRequest.hpp"

namespace rtype::network {

using testing::A_MILLISECOND_BEFORE_THE_WRAP;
using testing::ANSWERED_AT_FIFTY_MILLISECONDS;
using testing::ASKED_AT_FORTY_NINE_MILLISECONDS;
using testing::ASKED_AT_TEN_MILLISECONDS;
using testing::HELD_THREE_MILLISECONDS;
using testing::PAST_THE_WRAP;
using testing::PAST_THE_WRAP_ON_THE_WIRE;
using testing::THIRTY_SEVEN_MILLISECONDS;
using testing::THREE_MILLISECONDS;
using testing::TWENTY_ONE_SECONDS_LATER;
using testing::TWO_MILLISECONDS_AFTER_THE_WRAP;
using testing::WIRE_JITTER;
using testing::WIRE_JITTER_ON_THE_WIRE;
using testing::WIRE_PLAYER_NUMBER;
using testing::WIRE_ROUND_TRIP;
using testing::WIRE_ROUND_TRIP_ON_THE_WIRE;

/**
 * Given a time 1.5 µs past 2^32 µs
 * When it is written as a Heartbeat carries it
 * Then it is 1: whole microseconds, modulo 2^32
 */
TEST(RoundTripWire, KeepsTheWholeMicrosecondsModuloTwoToTheThirtyTwo) {
  EXPECT_EQ(wireMicrosecondsOf(PAST_THE_WRAP), PAST_THE_WRAP_ON_THE_WIRE);
}

/**
 * Given player 2 measured at 42.1 ms with a jitter of 3.2 ms
 * When its figures are written as RoundTrips carries them
 * Then they are 42100 µs and 3200 µs, under its number
 */
TEST(RoundTripWire, WritesTheFiguresOfAPlayerInMicroseconds) {
  EXPECT_EQ(playerRoundTripOf(WIRE_PLAYER_NUMBER,
                              RoundTripFigures{.roundTrip = WIRE_ROUND_TRIP,
                                               .jitter = WIRE_JITTER}),
            (PlayerRoundTrip{.playerNumber = WIRE_PLAYER_NUMBER,
                             .roundTrip = WIRE_ROUND_TRIP_ON_THE_WIRE,
                             .jitter = WIRE_JITTER_ON_THE_WIRE}));
}

/**
 * Given RoundTrips listing player 2 at 42100 µs with a jitter of 3200 µs
 * When its figures are read back
 * Then they are 42.1 ms and 3.2 ms
 */
TEST(RoundTripWire, ReadsTheFiguresOfAPlayerBack) {
  EXPECT_EQ(
      figuresOf(PlayerRoundTrip{.playerNumber = WIRE_PLAYER_NUMBER,
                                .roundTrip = WIRE_ROUND_TRIP_ON_THE_WIRE,
                                .jitter = WIRE_JITTER_ON_THE_WIRE}),
      (RoundTripFigures{.roundTrip = WIRE_ROUND_TRIP, .jitter = WIRE_JITTER}));
}

/**
 * Given a request sent at 10 ms, which the server held 3 ms
 * When its answer arrives at 50 ms
 * Then the round trip is 37 ms: the time the server held it is left out
 */
TEST(RoundTripWire, MeasuresTheRoundTripOfAnInformationRequest) {
  ServerInformation answer;
  answer.sentAt = informationRequestAt(ASKED_AT_TEN_MILLISECONDS).sentAt;
  answer.heldFor = HELD_THREE_MILLISECONDS;

  EXPECT_EQ(informationRoundTripOf(answer, ANSWERED_AT_FIFTY_MILLISECONDS),
            THIRTY_SEVEN_MILLISECONDS);
}

/**
 * Given a request sent 1 ms before the counter of microseconds wraps
 * When its answer arrives 2 ms after the wrap
 * Then the round trip is 3 ms: the wrap is counted modulo 2^32
 */
TEST(RoundTripWire, MeasuresARoundTripAcrossTheWrap) {
  ServerInformation answer;
  answer.sentAt = informationRequestAt(A_MILLISECOND_BEFORE_THE_WRAP).sentAt;

  EXPECT_EQ(informationRoundTripOf(answer, TWO_MILLISECONDS_AFTER_THE_WRAP),
            THREE_MILLISECONDS);
}

/**
 * Given an answer that says it was held 3 ms, for a request sent 1 ms before
 * it arrived
 * When the round trip is read
 * Then there is none: an answer cannot be held longer than its round trip
 */
TEST(RoundTripWire, MeasuresNothingHeldLongerThanSinceItWasSent) {
  ServerInformation answer;
  answer.sentAt = informationRequestAt(ASKED_AT_FORTY_NINE_MILLISECONDS).sentAt;
  answer.heldFor = HELD_THREE_MILLISECONDS;

  EXPECT_EQ(informationRoundTripOf(answer, ANSWERED_AT_FIFTY_MILLISECONDS),
            std::nullopt);
}

/**
 * Given a request sent at 10 ms
 * When its answer arrives 21 s later
 * Then there is no round trip: it is longer than the longest one, 20 s
 */
TEST(RoundTripWire, MeasuresNothingPastTheLongestRoundTrip) {
  ServerInformation answer;
  answer.sentAt = informationRequestAt(ASKED_AT_TEN_MILLISECONDS).sentAt;

  EXPECT_EQ(informationRoundTripOf(answer, TWENTY_ONE_SECONDS_LATER),
            std::nullopt);
}

}  // namespace rtype::network
