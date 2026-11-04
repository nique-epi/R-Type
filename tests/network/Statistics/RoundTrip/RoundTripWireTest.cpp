#include <gtest/gtest.h>
#include "RoundTripFigures.hpp"
#include "RoundTripWire.hpp"
#include "RoundTripWireTestConstants.hpp"
#include "RoundTrips.hpp"

namespace rtype::network {

using testing::PAST_THE_WRAP;
using testing::PAST_THE_WRAP_ON_THE_WIRE;
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

}  // namespace rtype::network
