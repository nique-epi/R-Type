#include "UdpPortProbe.hpp"
#include <cstdint>
#include <exception>
#include <string>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "UdpSocket.hpp"

namespace {

rtype::network::Endpoint anyInterfaceOn(std::uint16_t port) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::ANY_IPV4_ADDRESS}, .port = port};
}

}  // namespace

bool udpPortIsFree(std::uint16_t port) {
  try {
    rtype::network::NetworkContext network;
    const rtype::network::UdpSocket socket{network, anyInterfaceOn(port)};
    return true;
  } catch (const std::exception&) {
    return false;
  }
}

std::uint16_t freeUdpPort() {
  rtype::network::NetworkContext network;
  const rtype::network::UdpSocket socket{
      network, anyInterfaceOn(rtype::network::ANY_PORT)};
  return socket.localEndpoint().port;
}
