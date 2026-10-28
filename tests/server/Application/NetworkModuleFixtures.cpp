#include "NetworkModuleFixtures.hpp"
#include <atomic>
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "NetworkConstants.hpp"
#include "NetworkModule.hpp"
#include "RecordingModule.hpp"
#include "RecordingModuleSetup.hpp"
#include "ServerConstants.hpp"
#include "ServerTestConstants.hpp"
#include "TickCountingModule.hpp"
#include "TimeConstants.hpp"

std::vector<std::unique_ptr<rtype::engine::IModule>> journalNetworkThenCounter(
    JournalModule& journal,
    std::unique_ptr<rtype::server::NetworkModule> network,
    std::atomic<std::size_t>& passes) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<RecordingModule>(
      RecordingModuleSetup{
          .name = std::string{rtype::server::JOURNAL_MODULE_NAME},
          .dependencies = {}},
      journal));
  modules.push_back(std::move(network));
  modules.push_back(std::make_unique<TickCountingModule>(passes));
  return modules;
}

std::unique_ptr<rtype::server::NetworkModule> networkOnLoopback() {
  return std::make_unique<rtype::server::NetworkModule>(
      rtype::network::Endpoint{
          .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
          .port = rtype::network::ANY_PORT});
}

bool waitForPasses(const std::atomic<std::size_t>& passes) {
  const auto deadline = std::chrono::steady_clock::now() + THREAD_TIMEOUT;
  while (passes.load() < static_cast<std::size_t>(SERVER_RUN_TICKS)) {
    if (std::chrono::steady_clock::now() >= deadline) {
      return false;
    }
    std::this_thread::sleep_for(rtype::engine::SIMULATION_TICK_DURATION);
  }
  return true;
}
