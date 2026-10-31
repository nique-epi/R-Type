#include "AsioEndpoint.hpp"
#include <asio/ip/address.hpp>
#include <asio/ip/udp.hpp>
#include <system_error>
#include "Endpoint.hpp"
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

}  // namespace rtype::network
