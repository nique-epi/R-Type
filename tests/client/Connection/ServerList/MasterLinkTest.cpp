#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include "CreatedServer.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "GlobalLeaderboardReading.hpp"
#include "MasterLinkFixture.hpp"
#include "MasterLinkTestConstants.hpp"
#include "ServerListReading.hpp"
#include "ServerListing.hpp"

using rtype::client::GlobalLeaderboardReading;
using rtype::client::ServerListReading;
using rtype::client::testing::EMPTY_LEADERBOARD_DIFFICULTY;
using rtype::client::testing::EMPTY_LEADERBOARD_PILOTS;
using rtype::client::testing::EMPTY_LEADERBOARD_ROWS;
using rtype::client::testing::MasterLinkFixture;
using rtype::network::CreatedServer;
using rtype::network::GlobalLeaderboard;
using rtype::network::GlobalLeaderboardQuery;
using rtype::network::ServerListing;

/**
 * Given a running master where one server is registered
 * When the link asks for the servers
 * Then its thread reads that server, for that ask
 */
TEST_F(MasterLinkFixture, ReadsTheServersTheMasterListsOnItsThread) {
  const CreatedServer created = registeredServer();

  const std::uint64_t ask = link().askServers();
  const std::optional<ServerListReading> reading = serversSoon(ask);

  ASSERT_TRUE(reading.has_value());
  const ServerListing listing =
      reading.value_or(ServerListReading{}).listing.value_or(ServerListing{});
  ASSERT_EQ(listing.servers.size(), 1U);
  EXPECT_EQ(listing.servers.front().id, created.id);
}

/**
 * Given a master that stopped
 * When the link asks for the servers
 * Then the reading of that ask holds no listing, instead of never coming
 */
TEST_F(MasterLinkFixture, GivesNoListingWhenTheMasterDoesNotAnswer) {
  master().stop();

  const std::uint64_t ask = link().askServers();
  const std::optional<ServerListReading> reading = serversSoon(ask);

  ASSERT_TRUE(reading.has_value());
  EXPECT_FALSE(reading.value_or(ServerListReading{}).listing.has_value());
}

/**
 * Given a running master that kept no game
 * When the link asks for the leaderboard of two pilots in normal
 * Then its thread reads it, for that ask: a leaderboard with no row
 */
TEST_F(MasterLinkFixture, ReadsTheLeaderboardOfTheMasterOnItsThread) {
  const std::uint64_t ask = link().askLeaderboard(GlobalLeaderboardQuery{
      .difficulty = std::string{EMPTY_LEADERBOARD_DIFFICULTY},
      .playerCount = EMPTY_LEADERBOARD_PILOTS,
      .player = std::nullopt,
      .limit = EMPTY_LEADERBOARD_ROWS});
  const std::optional<GlobalLeaderboardReading> reading = leaderboardSoon(ask);

  ASSERT_TRUE(reading.has_value());
  EXPECT_EQ(reading.value_or(GlobalLeaderboardReading{}).leaderboard,
            std::optional<GlobalLeaderboard>{GlobalLeaderboard{}});
}
