#include "FreeUdpPort.hpp"
#include <cstdint>
#include <string>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "UdpSocket.hpp"

namespace rtype::server {

std::uint16_t freeUdpPort() {
  network::NetworkContext context;
  const network::UdpSocket socket{
      context,
      network::Endpoint{.address = std::string{network::ANY_IPV4_ADDRESS},
                        .port = network::ANY_PORT}};
  return socket.localEndpoint().port;
}

}  // namespace rtype::server
