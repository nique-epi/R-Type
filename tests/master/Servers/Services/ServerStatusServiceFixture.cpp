#include "ServerStatusServiceFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "KnownServer.hpp"
#include "ListedServer.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterTestConstants.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerMeasures.hpp"
#include "ServerRecord.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusChange.hpp"
#include "ServerStatusServiceTestConstants.hpp"
#include "StatusChangeListing.hpp"

rtype::master::ServerStatusService& ServerStatusServiceFixture::statuses() {
  return statuses_;
}

rtype::master::ServersService& ServerStatusServiceFixture::servers() {
  return servers_;
}

rtype::master::KnownServer ServerStatusServiceFixture::registeredAt(
    std::chrono::sys_seconds date) {
  const rtype::network::CreatedServer created =
      servers_
          .createServer(rtype::master::CreationAttempt{
              .creation =
                  rtype::network::ServerCreation{
                      .name = std::string{TEST_SERVER_NAME}},
              .address = std::string{STATUS_CREATOR_PREFIX} +
                         std::to_string(++creations_),
              .requestedAt = date})
          .value_or(rtype::network::CreatedServer{});
  servers_.replaceOwnServer(rtype::master::ServerRecord{
      .serverId = created.id,
      .registration = rtype::network::testing::parisRegistration(),
      .registeredAt = date});
  return servers_.serverOfKey(created.key)
      .value_or(rtype::master::KnownServer{});
}

void ServerStatusServiceFixture::beat(
    const rtype::master::KnownServer& server,
    const rtype::network::ServerMeasures& measures,
    std::chrono::sys_seconds date) {
  rtype::network::ServerHeartbeat heartbeat =
      rtype::network::testing::parisHeartbeat();
  heartbeat.measures = measures;
  static_cast<void>(servers_.replaceOwnHeartbeat(server, heartbeat, date));
}

std::optional<rtype::network::ServerStatus>
ServerStatusServiceFixture::listedStatusOf(std::int64_t serverId) {
  for (const rtype::network::ListedServer& listed :
       servers_.serverListingOf(WHOLE_PAGE).servers) {
    if (listed.id == serverId) {
      return listed.status;
    }
  }
  return std::nullopt;
}

std::vector<rtype::network::ServerStatusChange>
ServerStatusServiceFixture::historyOf(std::int64_t serverId) {
  return servers_.statusChangeListingOf(serverId, WHOLE_PAGE)
      .value_or(rtype::network::StatusChangeListing{})
      .statusChanges;
}

void ServerStatusServiceFixture::keepHeartbeatText(std::int64_t serverId,
                                                   std::string_view text) {
  databaseFile_.database().run([serverId, text](SQLite::Database& connection) {
    SQLite::Statement update{connection,
                             std::string{KEEP_HEARTBEAT_TEXT_STATEMENT}};
    update.bind(std::string{KEPT_TEXT_PARAMETER}, std::string{text});
    update.bind(std::string{KEPT_ID_PARAMETER}, serverId);
    update.exec();
  });
}
