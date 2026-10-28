#include "PortProbeModule.hpp"
#include <cstdint>
#include <exception>
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "InstallContext.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "UdpSocket.hpp"

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
  try {
    rtype::network::NetworkContext network;
    const rtype::network::UdpSocket socket{
        network, rtype::network::Endpoint{
                     .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
                     .port = port_}};
    portWasFree_ = true;
  } catch (const std::exception&) {
    portWasFree_ = false;
  }
}

void PortProbeModule::start() {}

void PortProbeModule::stop() {}

bool PortProbeModule::portWasFree() const { return portWasFree_; }
