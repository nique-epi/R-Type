#include "ServerStatusServiceFixture.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "DatedStatus.hpp"
#include "KnownServer.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServerStatusService.hpp"
#include "ServerStatusServiceTestConstants.hpp"
#include "ServersFixtures.hpp"
#include "ServersService.hpp"
#include "ServersServiceTestConstants.hpp"
#include "StatusListing.hpp"

rtype::master::ServerStatusService& ServerStatusServiceFixture::statuses() {
  return statuses_;
}

rtype::master::ServersService& ServerStatusServiceFixture::servers() {
  return servers_;
}

rtype::master::KnownServer ServerStatusServiceFixture::registeredAt(
    std::chrono::sys_seconds date) {
  return registeredParis(servers_, nextCreatorAddress(), date);
}

rtype::master::KnownServer ServerStatusServiceFixture::registeredElsewhereAt(
    std::chrono::sys_seconds date) {
  const std::string key =
      keyOfRegisteredParis(servers_, nextCreatorAddress(), date);
  rtype::network::ServerRegistration elsewhere =
      rtype::network::testing::parisRegistration();
  elsewhere.udpPort = OTHER_UDP_PORT;
  servers_.replaceOwnServer(rtype::master::ServerRecord{
      .serverId =
          servers_.serverOfKey(key).value_or(rtype::master::KnownServer{}).id,
      .registration = elsewhere,
      .registeredAt = date});
  return servers_.serverOfKey(key).value_or(rtype::master::KnownServer{});
}

void ServerStatusServiceFixture::beat(const rtype::master::KnownServer& server,
                                      double tickP95,
                                      std::chrono::sys_seconds date) {
  rtype::network::ServerHeartbeat heartbeat =
      answeringHeartbeat(servers_, server, date);
  heartbeat.measures.tickP95Milliseconds = tickP95;
  static_cast<void>(servers_.replaceOwnHeartbeat(server, heartbeat, date));
}

void ServerStatusServiceFixture::beatWithoutAnswer(
    const rtype::master::KnownServer& server, std::chrono::sys_seconds date) {
  static_cast<void>(servers_.replaceOwnHeartbeat(
      server, rtype::network::testing::parisHeartbeat(), date));
}

void ServerStatusServiceFixture::registerAgain(std::int64_t serverId,
                                               std::chrono::sys_seconds date) {
  servers_.replaceOwnServer(rtype::master::ServerRecord{
      .serverId = serverId,
      .registration = rtype::network::testing::parisRegistration(),
      .registeredAt = date});
}

void ServerStatusServiceFixture::execute(std::string_view statement) {
  executeStatement(databaseFile_.database(), statement);
}

std::vector<rtype::network::DatedStatus> ServerStatusServiceFixture::historyOf(
    std::int64_t serverId) {
  return servers_.statusListingOf(serverId, WHOLE_PAGE)
      .value_or(rtype::network::StatusListing{})
      .statuses;
}

std::string ServerStatusServiceFixture::nextCreatorAddress() {
  return std::string{CREATOR_ADDRESS_PREFIX} + std::to_string(++creations_);
}
