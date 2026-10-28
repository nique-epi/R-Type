#include "TickCountingModule.hpp"
#include <atomic>
#include <cstddef>
#include <string>
#include <vector>
#include "InstallContext.hpp"
#include "ServerConstants.hpp"
#include "TickCountingModuleConstants.hpp"
#include "TimeConstants.hpp"

TickCountingModule::TickCountingModule(std::atomic<std::size_t>& passes)
    : passes_(&passes) {}

std::string TickCountingModule::name() const { return TICK_COUNTING_MODULE; }

std::vector<std::string> TickCountingModule::dependencies() const {
  return {std::string{rtype::server::NETWORK_MODULE_NAME}};
}

void TickCountingModule::install(rtype::engine::InstallContext& context) {
  context.addWork(
      rtype::server::SIMULATION_STAGE,
      [this]([[maybe_unused]] rtype::engine::Duration elapsed) { ++*passes_; });
}

void TickCountingModule::start() {}

void TickCountingModule::stop() {}
