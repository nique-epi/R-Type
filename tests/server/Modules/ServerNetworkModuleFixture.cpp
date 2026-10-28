#include "ServerNetworkModuleFixture.hpp"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "IModule.hpp"
#include "ModuleConstants.hpp"
#include "ServerJournalModule.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerNetworkModule.hpp"
#include "ServiceProbeModule.hpp"
#include "StageWork.hpp"
#include "StopRequest.hpp"
#include "TimeConstants.hpp"

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerNetworkModuleFixture::networkModulesThen(
    std::uint16_t port, std::unique_ptr<rtype::engine::IModule> probe) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(
      std::make_unique<rtype::server::ServerJournalModule>(journalOptions()));
  modules.push_back(
      std::make_unique<rtype::server::ServerNetworkModule>(port, stopRequest_));
  modules.push_back(std::move(probe));
  return modules;
}

std::unique_ptr<ServiceProbeModule> ServerNetworkModuleFixture::probeRunning(
    rtype::engine::StageWork work) {
  return std::make_unique<ServiceProbeModule>(
      PROBE_MODULE,
      std::vector<std::string>{std::string{rtype::server::NETWORK_MODULE_NAME}},
      std::move(work));
}

rtype::engine::StageWork ServerNetworkModuleFixture::countingPassesIn(
    std::atomic<int>& passes,
    std::atomic<std::int64_t>& lastElapsedNanoseconds) {
  return [&passes, &lastElapsedNanoseconds](rtype::engine::Duration elapsed) {
    lastElapsedNanoseconds = elapsed.count();
    ++passes;
  };
}

bool ServerNetworkModuleFixture::waitForPasses(const std::atomic<int>& passes,
                                               int wanted) {
  const auto deadline = std::chrono::steady_clock::now() + WAIT_FOR_THREADS;
  while (passes < wanted) {
    if (std::chrono::steady_clock::now() > deadline) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return true;
}

rtype::server::StopRequest& ServerNetworkModuleFixture::stopRequest() {
  return stopRequest_;
}
