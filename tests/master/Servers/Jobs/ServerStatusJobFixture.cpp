#include "ServerStatusJobFixture.hpp"
#include <chrono>
#include <cstdint>
#include <thread>
#include "KnownServer.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusJobTestConstants.hpp"
#include "ServersFixtures.hpp"

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
  const rtype::master::KnownServer server =
      registeredParis(servers_, JOB_CREATOR_ADDRESS, now);
  static_cast<void>(servers_.replaceOwnHeartbeat(
      server, answeringHeartbeat(servers_, server, now), now));
  return server.id;
}

bool ServerStatusJobFixture::statusSoonIs(std::int64_t serverId,
                                          rtype::network::ServerStatus status) {
  const auto deadline = std::chrono::steady_clock::now() + STATUS_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    if (listedStatusOf(servers_, serverId) == status) {
      return true;
    }
    std::this_thread::sleep_for(STATUS_POLL);
  }
  return false;
}
