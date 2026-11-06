#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "Endpoint.hpp"
#include "ServerCatalogFixture.hpp"
#include "ServerCatalogTestConstants.hpp"
#include "ServerCompatibility.hpp"
#include "ServerListPhase.hpp"
#include "ServerListing.hpp"
#include "ServerRow.hpp"
#include "ServerStatus.hpp"

using rtype::client::ServerCompatibility;
using rtype::client::ServerListPhase;
using rtype::client::ServerRow;
using rtype::client::testing::BREST_ID;
using rtype::client::testing::BREST_SERVER_ADDRESS;
using rtype::client::testing::HIGH_PING;
using rtype::client::testing::JUST_BEFORE_GIVE_UP;
using rtype::client::testing::LOW_PING;
using rtype::client::testing::LYON_ID;
using rtype::client::testing::LYON_SERVER_ADDRESS;
using rtype::client::testing::NANTES_ID;
using rtype::client::testing::NANTES_SERVER_ADDRESS;
using rtype::client::testing::ONE_FRAME;
using rtype::client::testing::OTHER_PROTOCOL_VERSION;
using rtype::client::testing::PARIS_ID;
using rtype::client::testing::PARIS_SERVER_ADDRESS;
using rtype::client::testing::ROUND_GIVE_UP;
using rtype::client::testing::SERVER_MAXIMUM_PLAYERS;
using rtype::client::testing::ServerCatalogFixture;
using rtype::network::Endpoint;
using rtype::network::ServerListing;
using rtype::network::ServerStatus;

/**
 * Given a catalog that asked the master
 * When the master has not answered yet
 * Then the list is searching
 */
TEST_F(ServerCatalogFixture, IsSearchingUntilTheMasterAnswers) {
  catalog().refresh();
  update(ONE_FRAME);

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::Searching);
}

/**
 * Given a catalog that asked the master
 * When the master does not answer the ask
 * Then the list says the master is unreachable, with no server
 */
TEST_F(ServerCatalogFixture, SaysTheMasterIsUnreachableWhenItDidNotAnswer) {
  catalog().refresh();
  master().answerServers(std::nullopt);
  update(ONE_FRAME);

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::MasterUnreachable);
  EXPECT_TRUE(catalog().snapshot().servers.empty());
}

/**
 * Given a list of four servers, online, degraded, offline and in maintenance
 * When the catalog takes it
 * Then it pings the online and the degraded ones only
 */
TEST_F(ServerCatalogFixture, PingsTheOnlineAndDegradedServersOnly) {
  listAndUpdate(ServerListing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS}),
                  listed({.id = LYON_ID,
                          .address = LYON_SERVER_ADDRESS,
                          .status = ServerStatus::Degraded}),
                  listed({.id = BREST_ID,
                          .address = BREST_SERVER_ADDRESS,
                          .status = ServerStatus::Offline}),
                  listed({.id = NANTES_ID,
                          .address = NANTES_SERVER_ADDRESS,
                          .status = ServerStatus::Maintenance})}});

  ASSERT_EQ(pings().rounds().size(), 1U);
  EXPECT_EQ(pings().rounds().front(),
            (std::vector<Endpoint>{endpointAt(PARIS_SERVER_ADDRESS),
                                   endpointAt(LYON_SERVER_ADDRESS)}));
}

/**
 * Given a list of one server, which answers the round
 * When the round closes
 * Then the list holds that server with its ping and its bounds
 */
TEST_F(ServerCatalogFixture, ListsAServerThatAnsweredWithItsPing) {
  listAndUpdate(ServerListing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS})}});
  answerFrom(PARIS_SERVER_ADDRESS, LOW_PING);
  update(ONE_FRAME);

  ASSERT_EQ(catalog().snapshot().phase, ServerListPhase::Ready);
  ASSERT_EQ(catalog().snapshot().servers.size(), 1U);
  const ServerRow& paris = catalog().snapshot().servers.front();
  EXPECT_EQ(paris.id, PARIS_ID);
  EXPECT_EQ(paris.ping, std::optional{LOW_PING});
  EXPECT_EQ(paris.maximumPlayers, SERVER_MAXIMUM_PLAYERS);
}

/**
 * Given a round to Paris and Lyon, which only Paris answered
 * When the round is 1.996 s old, less than the round waits
 * Then the list still shows nothing new
 */
TEST_F(ServerCatalogFixture, WaitsForTheRoundBeforeShowingTheList) {
  listAndUpdate(ServerListing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS}),
                  listed({.id = LYON_ID, .address = LYON_SERVER_ADDRESS})}});
  answerFrom(PARIS_SERVER_ADDRESS, LOW_PING);
  update(JUST_BEFORE_GIVE_UP);

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::Searching);
}

/**
 * Given a round to Paris and Lyon, which only Paris answered
 * When 2 s pass
 * Then the list holds Paris alone: Lyon is unreachable
 */
TEST_F(ServerCatalogFixture, LeavesOutAServerThatNeverAnswered) {
  listAndUpdate(ServerListing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS}),
                  listed({.id = LYON_ID, .address = LYON_SERVER_ADDRESS})}});
  answerFrom(PARIS_SERVER_ADDRESS, LOW_PING);
  update(ROUND_GIVE_UP);

  ASSERT_EQ(catalog().snapshot().servers.size(), 1U);
  EXPECT_EQ(catalog().snapshot().servers.front().id, PARIS_ID);
}

/**
 * Given a list of one server of another version of the protocol
 * When the catalog takes it
 * Then it is listed at once, needing an update, with no ping, and not pinged
 */
TEST_F(ServerCatalogFixture, ListsAServerOfAnotherVersionWithNoPing) {
  ServerListing listing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS})}};
  listing.servers.front().registration.protocolVersion = OTHER_PROTOCOL_VERSION;
  listAndUpdate(listing);

  ASSERT_EQ(catalog().snapshot().servers.size(), 1U);
  const ServerRow& paris = catalog().snapshot().servers.front();
  EXPECT_EQ(paris.compatibility, ServerCompatibility::UpdateNeeded);
  EXPECT_FALSE(paris.ping.has_value());
  EXPECT_TRUE(pings().rounds().front().empty());
}

/**
 * Given Lyon answering with 24 ms and Paris with 88 ms
 * When the round closes
 * Then Lyon comes first
 */
TEST_F(ServerCatalogFixture, SortsTheServersByPing) {
  listAndUpdate(ServerListing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS}),
                  listed({.id = LYON_ID, .address = LYON_SERVER_ADDRESS})}});
  answerFrom(PARIS_SERVER_ADDRESS, HIGH_PING);
  answerFrom(LYON_SERVER_ADDRESS, LOW_PING);
  update(ONE_FRAME);

  ASSERT_EQ(catalog().snapshot().servers.size(), 2U);
  EXPECT_EQ(catalog().snapshot().servers.front().id, LYON_ID);
}

/**
 * Given a list showing Paris
 * When the catalog refreshes and the new round has not closed
 * Then Paris is still shown
 */
TEST_F(ServerCatalogFixture, KeepsTheListUntilTheNewRoundCloses) {
  const ServerListing listing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS})}};
  listAndUpdate(listing);
  answerFrom(PARIS_SERVER_ADDRESS, LOW_PING);
  update(ONE_FRAME);
  listAndUpdate(listing);

  ASSERT_EQ(catalog().snapshot().servers.size(), 1U);
  EXPECT_EQ(catalog().snapshot().servers.front().id, PARIS_ID);
}

/**
 * Given the master answered a first ask, and the catalog asked again since
 * When a frame runs
 * Then the answer to the first ask starts no round
 */
TEST_F(ServerCatalogFixture, IgnoresTheAnswerToAnOlderAsk) {
  catalog().refresh();
  master().answerServers(ServerListing{
      .servers = {listed({.id = PARIS_ID, .address = PARIS_SERVER_ADDRESS})}});
  catalog().refresh();
  update(ONE_FRAME);

  EXPECT_TRUE(pings().rounds().empty());
}

/**
 * Given a list whose only server is offline
 * When the catalog takes it
 * Then the list is empty
 */
TEST_F(ServerCatalogFixture, IsEmptyWhenNoServerCanBeListed) {
  listAndUpdate(
      ServerListing{.servers = {listed({.id = PARIS_ID,
                                        .address = PARIS_SERVER_ADDRESS,
                                        .status = ServerStatus::Offline})}});

  EXPECT_EQ(catalog().snapshot().phase, ServerListPhase::Empty);
}
