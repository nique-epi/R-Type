#include "ServersFixtures.hpp"
#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "DatagramCodec.hpp"
#include "KnownServer.hpp"
#include "ListedServer.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterProbe.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PageRequest.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRecord.hpp"
#include "ServerStatus.hpp"
#include "ServersService.hpp"

std::string keyOfRegisteredParis(rtype::master::ServersService& servers,
                                 std::string_view address,
                                 std::chrono::sys_seconds date) {
  const rtype::network::CreatedServer created =
      servers
          .createServer(rtype::master::CreationAttempt{
              .creation =
                  rtype::network::ServerCreation{
                      .name = std::string{TEST_SERVER_NAME}},
              .address = std::string{address},
              .requestedAt = date})
          .value_or(rtype::network::CreatedServer{});
  servers.replaceOwnServer(rtype::master::ServerRecord{
      .serverId = created.id,
      .registration = rtype::network::testing::parisRegistration(),
      .registeredAt = date});
  return created.key;
}

rtype::master::KnownServer registeredParis(
    rtype::master::ServersService& servers, std::string_view address,
    std::chrono::sys_seconds date) {
  return servers.serverOfKey(keyOfRegisteredParis(servers, address, date))
      .value_or(rtype::master::KnownServer{});
}

void executeStatement(rtype::master::MasterDatabase& database,
                      std::string_view statement) {
  database.run([statement](SQLite::Database& connection) {
    connection.exec(std::string{statement});
  });
}

std::optional<rtype::network::ServerStatus> listedStatusOf(
    rtype::master::ServersService& servers, std::int64_t serverId) {
  for (const rtype::network::ListedServer& listed :
       servers
           .serverListingOf(rtype::master::PageRequest{
               .limit = rtype::network::MAXIMUM_PAGE_SIZE,
               .offset = rtype::network::FIRST_PAGE_OFFSET})
           .servers) {
    if (listed.id == serverId) {
      return listed.status;
    }
  }
  return std::nullopt;
}

rtype::network::ServerHeartbeat answeringHeartbeat(
    const rtype::master::ServersService& servers,
    const rtype::master::KnownServer& server, std::chrono::sys_seconds date) {
  rtype::network::ServerHeartbeat heartbeat =
      rtype::network::testing::parisHeartbeat();
  const std::optional<rtype::network::OutgoingDatagram> probe =
      servers.probeFor(server, date);
  if (probe.has_value()) {
    heartbeat.probeToken = rtype::network::DatagramCodec::firstMessage<
                               rtype::network::MasterProbe>(probe->payload)
                               .value_or(rtype::network::MasterProbe{})
                               .token;
  }
  return heartbeat;
}
