#include "MasterProbeModule.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "IProbeSender.hpp"
#include "InstallContext.hpp"
#include "MasterConstants.hpp"
#include "MasterModuleConstants.hpp"

namespace rtype::master {

MasterProbeModule::MasterProbeModule()
    : logger_(std::string{MASTER_LOGGER_NAME}) {}

std::string MasterProbeModule::name() const {
  return std::string{PROBE_MODULE_NAME};
}

std::vector<std::string> MasterProbeModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void MasterProbeModule::install(engine::InstallContext& context) {
  context.provide<IProbeSender>(sender_);
}

void MasterProbeModule::start() {
  const std::uint16_t port = sender_.open();
  logger_.info("probes leave from UDP port ", port);
}

void MasterProbeModule::stop() { sender_.close(); }

}  // namespace rtype::master
