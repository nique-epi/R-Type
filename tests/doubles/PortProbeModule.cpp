#include "PortProbeModule.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "UdpPortProbe.hpp"

PortProbeModule::PortProbeModule(std::string name,
                                 std::vector<std::string> dependencies,
                                 std::uint16_t port)
    : name_(std::move(name)),
      dependencies_(std::move(dependencies)),
      port_(port) {}

std::string PortProbeModule::name() const { return name_; }

std::vector<std::string> PortProbeModule::dependencies() const {
  return dependencies_;
}

void PortProbeModule::install(
    [[maybe_unused]] rtype::engine::InstallContext& context) {
  portWasFree_ = udpPortIsFree(port_);
}

void PortProbeModule::start() {}

void PortProbeModule::stop() {}

bool PortProbeModule::portWasFree() const { return portWasFree_; }
