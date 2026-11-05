#include "ServerStatusJobFixture.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
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
#include "ServerStatusJobTestConstants.hpp"

namespace {

std::chrono::sys_seconds nowInSeconds() {
  return std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

}  // namespace

void ServerStatusJobFixture::startJob(std::chrono::milliseconds interval) {
  job_.emplace(databaseFile_.database(), interval);
}

void ServerStatusJobFixture::stopJob() { job_.reset(); }

std::int64_t ServerStatusJobFixture::serverBeatingNow() {
  const std::chrono::sys_seconds now = nowInSeconds();
  const rtype::network::CreatedServer created =
      servers_
          .createServer(rtype::master::CreationAttempt{
              .creation =
                  rtype::network::ServerCreation{
                      .name = std::string{TEST_SERVER_NAME}},
              .address = std::string{JOB_CREATOR_ADDRESS},
              .requestedAt = now})
          .value_or(rtype::network::CreatedServer{});
  servers_.replaceOwnServer(rtype::master::ServerRecord{
      .serverId = created.id,
      .registration = rtype::network::testing::parisRegistration(),
      .registeredAt = now});
  static_cast<void>(servers_.replaceOwnHeartbeat(
      servers_.serverOfKey(created.key).value_or(rtype::master::KnownServer{}),
      rtype::network::testing::parisHeartbeat(), now));
  return created.id;
}

bool ServerStatusJobFixture::statusSoonIs(std::int64_t serverId,
                                          rtype::network::ServerStatus status) {
  const auto deadline = std::chrono::steady_clock::now() + STATUS_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    for (const rtype::network::ListedServer& listed :
         servers_
             .serverListingOf(rtype::master::PageRequest{
                 .limit = rtype::network::MAXIMUM_PAGE_SIZE,
                 .offset = rtype::network::FIRST_PAGE_OFFSET})
             .servers) {
      if (listed.id == serverId && listed.status == status) {
        return true;
      }
    }
    std::this_thread::sleep_for(STATUS_POLL);
  }
  return false;
}
