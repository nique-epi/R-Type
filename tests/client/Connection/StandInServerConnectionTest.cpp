#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <string_view>
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "StandInServerConnection.hpp"

using rtype::client::ConnectionStatus;
using rtype::client::StandInServerConnection;
using rtype::network::Endpoint;

namespace {

/** @brief Address and port of any server: the stand-in contacts none. */
constexpr std::string_view ANY_ADDRESS = "192.168.1.20";
constexpr std::uint16_t ANY_PORT = 4243;

/** @returns Any server: the stand-in contacts none. */
Endpoint anyServer() {
  return {.address = std::string(ANY_ADDRESS), .port = ANY_PORT};
}

/** @brief The number of the first player of a game, as the protocol page gives
 * it. */
constexpr std::uint8_t FIRST_PLAYER = 1;

}  // namespace

/**
 * Given a stand-in connection just built
 * When nothing happens
 * Then no attempt is in progress
 */
TEST(StandInServerConnection, StartsWithoutAttempt) {
  const StandInServerConnection connection;

  EXPECT_EQ(connection.status().phase, ConnectionStatus::Phase::Idle);
}

/**
 * Given a stand-in connection
 * When an attempt starts
 * Then it is accepted at once, as the first player
 */
TEST(StandInServerConnection, AcceptsAtOnceAsTheFirstPlayer) {
  StandInServerConnection connection;

  connection.connect(anyServer(), "ACE");

  EXPECT_EQ(connection.status(),
            (ConnectionStatus{.phase = ConnectionStatus::Phase::Accepted,
                              .playerNumber = FIRST_PLAYER}));
}

/**
 * Given a stand-in connection that accepted an attempt
 * When the attempt is dropped
 * Then no attempt is in progress
 */
TEST(StandInServerConnection, CancelDropsTheAcceptance) {
  StandInServerConnection connection;
  connection.connect(anyServer(), "ACE");

  connection.cancel();

  EXPECT_EQ(connection.status().phase, ConnectionStatus::Phase::Idle);
}
