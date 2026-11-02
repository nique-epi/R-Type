#include "AsioEndpoint.hpp"
#include <asio/ip/address.hpp>
#include <asio/ip/udp.hpp>
#include <cstddef>
#include <span>
#include <system_error>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"

namespace rtype::network {

asio::ip::udp::endpoint toAsioEndpoint(const Endpoint& endpoint) {
  std::error_code error;
  const asio::ip::address address =
      asio::ip::make_address(endpoint.address, error);
  if (error) {
    throw InvalidAddressException(endpoint.address);
  }
  return {address, endpoint.port};
}

asio::ip::udp::endpoint sendableDestination(
    const Endpoint& destination, std::span<const std::byte> payload) {
  if (payload.size() > MAX_DATAGRAM_SIZE) {
    throw DatagramTooLargeException(payload.size());
  }
  return toAsioEndpoint(destination);
}

}  // namespace rtype::network
