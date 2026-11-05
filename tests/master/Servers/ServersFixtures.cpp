#include "ServersFixtures.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "KnownServer.hpp"
#include "ListedServer.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterTestConstants.hpp"
#include "PageRequest.hpp"
#include "ServerCreation.hpp"
#include "ServerRecord.hpp"
#include "ServerStatus.hpp"
#include "ServersService.hpp"

rtype::master::KnownServer registeredParis(
    rtype::master::ServersService& servers, std::string_view address,
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
  return servers.serverOfKey(created.key)
      .value_or(rtype::master::KnownServer{});
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
