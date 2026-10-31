#pragma once

#include <asio/ip/udp.hpp>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @returns @p endpoint as Asio names it.
 * @throws InvalidAddressException when the address of @p endpoint is not an
 *         IP address.
 */
[[nodiscard]] asio::ip::udp::endpoint toAsioEndpoint(const Endpoint& endpoint);

}  // namespace rtype::network
