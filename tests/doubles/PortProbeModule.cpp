#include "PortProbeModule.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "IListeningPort.hpp"
#include "InstallContext.hpp"

PortProbeModule::PortProbeModule(std::string name,
                                 std::vector<std::string> dependencies)
    : name_(std::move(name)), dependencies_(std::move(dependencies)) {}

std::string PortProbeModule::name() const { return name_; }

std::vector<std::string> PortProbeModule::dependencies() const {
  return dependencies_;
}

void PortProbeModule::install(rtype::engine::InstallContext& context) {
  portAtInstall_ =
      context.require<rtype::server::IListeningPort>().listeningPort();
}

void PortProbeModule::start() {}

void PortProbeModule::stop() {}

std::uint16_t PortProbeModule::portAtInstall() const { return portAtInstall_; }
