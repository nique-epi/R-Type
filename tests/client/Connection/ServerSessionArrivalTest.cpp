#include <gtest/gtest.h>
#include <optional>
#include "Heartbeat.hpp"
#include "RoundTripFigures.hpp"
#include "ServerSessionArrivalFixture.hpp"
#include "ServerSessionArrivalTestConstants.hpp"

namespace rtype::client {

using testing::ECHO_HELD_BY_THE_SERVER;
using testing::ECHO_STAMPED_AT;
using testing::FIRST_HEARTBEAT_TIME;
using testing::ROUND_TRIP_OF_THE_ECHO;
using testing::ServerSessionArrivalFixture;

/**
 * Given a session that sent its first Heartbeat 500 ms after it opened
 * When the echo of that Heartbeat, held 2 ms by the server, comes in a
 * datagram stamped 50 ms after the Heartbeat left
 * Then the round trip of the player is 48 ms: the session hands the time its
 * datagram arrived to each message it holds
 */
TEST_F(ServerSessionArrivalFixture, MeasuresWithTheTimeTheDatagramArrived) {
  openSession();
  update(FIRST_HEARTBEAT_TIME);
  const network::Heartbeat sent = lastSentHeartbeat();

  deliverStamped(
      network::Heartbeat{
          .sentAt = 0,
          .echo = network::HeartbeatEcho{.sentAt = sent.sentAt,
                                         .heldFor = ECHO_HELD_BY_THE_SERVER}},
      ECHO_STAMPED_AT);

  const std::optional<network::RoundTripFigures> figures = ownRoundTrip();
  ASSERT_TRUE(figures.has_value());
  EXPECT_EQ(figures.value_or(network::RoundTripFigures{}).roundTrip,
            ROUND_TRIP_OF_THE_ECHO);
}

}  // namespace rtype::client
