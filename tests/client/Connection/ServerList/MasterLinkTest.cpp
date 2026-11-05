#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "CreatedServer.hpp"
#include "MasterLinkFixture.hpp"
#include "ServerListReading.hpp"
#include "ServerListing.hpp"

using rtype::client::ServerListReading;
using rtype::client::testing::MasterLinkFixture;
using rtype::network::CreatedServer;
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
