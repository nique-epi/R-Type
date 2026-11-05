#include "ServerStatusServiceFixture.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>
#include "KnownServer.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRecord.hpp"
#include "ServerStatusChange.hpp"
#include "ServerStatusService.hpp"
#include "ServerStatusServiceTestConstants.hpp"
#include "ServersFixtures.hpp"
#include "ServersService.hpp"
#include "ServersServiceTestConstants.hpp"
#include "StatusChangeListing.hpp"

rtype::master::ServerStatusService& ServerStatusServiceFixture::statuses() {
  return statuses_;
}

rtype::master::ServersService& ServerStatusServiceFixture::servers() {
  return servers_;
}

rtype::master::KnownServer ServerStatusServiceFixture::registeredAt(
    std::chrono::sys_seconds date) {
  return registeredParis(
      servers_,
      std::string{CREATOR_ADDRESS_PREFIX} + std::to_string(++creations_), date);
}

void ServerStatusServiceFixture::beat(const rtype::master::KnownServer& server,
                                      double tickP95,
                                      std::chrono::sys_seconds date) {
  rtype::network::ServerHeartbeat heartbeat =
      rtype::network::testing::parisHeartbeat();
  heartbeat.measures.tickP95Milliseconds = tickP95;
  static_cast<void>(servers_.replaceOwnHeartbeat(server, heartbeat, date));
}

void ServerStatusServiceFixture::registerAgain(std::int64_t serverId,
                                               std::chrono::sys_seconds date) {
  servers_.replaceOwnServer(rtype::master::ServerRecord{
      .serverId = serverId,
      .registration = rtype::network::testing::parisRegistration(),
      .registeredAt = date});
}

std::vector<rtype::network::ServerStatusChange>
ServerStatusServiceFixture::historyOf(std::int64_t serverId) {
  return servers_.statusChangeListingOf(serverId, WHOLE_PAGE)
      .value_or(rtype::network::StatusChangeListing{})
      .statusChanges;
}
