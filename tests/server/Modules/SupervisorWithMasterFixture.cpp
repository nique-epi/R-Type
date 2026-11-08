#include "SupervisorWithMasterFixture.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include "ListedServer.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "RunningMaster.hpp"
#include "ServerListing.hpp"
#include "SupervisorModulesTestConstants.hpp"

const RunningMaster& SupervisorWithMasterFixture::master() const {
  return master_;
}

std::optional<std::string> SupervisorWithMasterFixture::listedAddressSoon(
    std::int64_t serverId) const {
  rtype::network::MasterClient anyone{
      rtype::network::MasterAccess{.url = master_.url(), .key = std::string{}}};
  const auto deadline =
      std::chrono::steady_clock::now() + REGISTRATION_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    for (const rtype::network::ListedServer& server :
         anyone.listServers().servers) {
      if (server.id == serverId) {
        return server.registration.address;
      }
    }
    std::this_thread::sleep_for(LISTING_WAIT_STEP);
  }
  return std::nullopt;
}
