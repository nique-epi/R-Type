#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include "EntityType.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "StateRateMeasurementFixtures.hpp"
#include "StateRateMeasurementTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldStateSenderFixture.hpp"
#include "WorldStateSenderTestConstants.hpp"

using rtype::game::EntityType;
using rtype::server::testing::bytesSentTo;
using rtype::server::testing::client;
using rtype::server::testing::FIRST_CLIENT_PORT;
using rtype::server::testing::longestGapInTicks;
using rtype::server::testing::MEASURED_ENTITIES;
using rtype::server::testing::MEASURED_PLAYERS;
using rtype::server::testing::MEASURED_SECONDS;
using rtype::server::testing::MEASURED_STATE_RATES;
using rtype::server::testing::MILLISECONDS_PER_SECOND;
using rtype::server::testing::RATIO_TOLERANCE;
using rtype::server::testing::REFERENCE_RATE_INDEX;
using rtype::server::testing::SESSION_TOKEN;
using rtype::server::testing::SHIP_POSITION;
using rtype::server::testing::ticksSentTo;
using rtype::server::testing::ticksThatSurvive;
using rtype::server::testing::TRIPLE;
using rtype::server::testing::TRIPLE_RATE_INDEX;
using rtype::server::testing::WorldStateSenderFixture;

/**
 * Given a full room and a screen of networked entities
 * When ten seconds of game run at several state rates
 * Then the bytes sent to a player each second grow with the rate, and three
 * times the reference rate costs three times its bytes
 */
TEST_F(WorldStateSenderFixture, BandwidthGrowsInProportionToTheStateRate) {
  for (std::uint32_t id = 1; id <= MEASURED_ENTITIES; ++id) {
    addNetworkedEntity(EntityType::Enemy, SHIP_POSITION, id);
  }
  for (std::size_t player = 0; player < MEASURED_PLAYERS; ++player) {
    connect(static_cast<std::uint16_t>(FIRST_CLIENT_PORT + player),
            SESSION_TOKEN + player);
  }
  std::vector<double> bytesPerSecond;

  for (const std::size_t rate : MEASURED_STATE_RATES) {
    restartWithStatesPerSecond(rate);
    runTicks(MEASURED_SECONDS * rtype::engine::SIMULATION_TICKS_PER_SECOND);
    bytesPerSecond.push_back(static_cast<double>(bytesSentTo(
                                 sentDatagrams(), client(FIRST_CLIENT_PORT))) /
                             static_cast<double>(MEASURED_SECONDS));
    std::cout << rate << " states/s: " << bytesPerSecond.back()
              << " bytes/s to one player\n";
  }

  for (std::size_t index = 1; index < bytesPerSecond.size(); ++index) {
    EXPECT_GT(bytesPerSecond[index], bytesPerSecond[index - 1]);
  }
  const double reference = bytesPerSecond[REFERENCE_RATE_INDEX];
  const double triple = bytesPerSecond[TRIPLE_RATE_INDEX];
  EXPECT_NEAR(triple / reference, TRIPLE, TRIPLE * RATIO_TOLERANCE);
}

/**
 * Given a full room and a screen of networked entities
 * When ten seconds of game run at several state rates
 * Then the longest time without a state is one rate-th of a second rounded up
 * to whole ticks, and one datagram in ten lost never shortens it
 */
TEST_F(WorldStateSenderFixture, UpdateGapShrinksAsTheStateRateGrows) {
  for (std::uint32_t id = 1; id <= MEASURED_ENTITIES; ++id) {
    addNetworkedEntity(EntityType::Enemy, SHIP_POSITION, id);
  }
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  const auto lastTick = static_cast<std::uint32_t>(
      MEASURED_SECONDS * rtype::engine::SIMULATION_TICKS_PER_SECOND);

  for (const std::size_t rate : MEASURED_STATE_RATES) {
    SCOPED_TRACE(rate);
    restartWithStatesPerSecond(rate);
    runTicks(lastTick);
    const std::vector<std::uint32_t> ticks =
        ticksSentTo(sentDatagrams(), client(FIRST_CLIENT_PORT));

    const std::uint32_t gap = longestGapInTicks(ticks, lastTick);
    const std::uint32_t gapWithLoss =
        longestGapInTicks(ticksThatSurvive(ticks), lastTick);

    std::cout << rate << " states/s: longest gap "
              << gap * MILLISECONDS_PER_SECOND /
                     rtype::engine::SIMULATION_TICKS_PER_SECOND
              << " ms, "
              << gapWithLoss * MILLISECONDS_PER_SECOND /
                     rtype::engine::SIMULATION_TICKS_PER_SECOND
              << " ms with one datagram in ten lost\n";
    EXPECT_EQ(gap,
              (rtype::engine::SIMULATION_TICKS_PER_SECOND + rate - 1) / rate);
    EXPECT_GE(gapWithLoss, gap);
  }
}
