#pragma once

#include <asio/ip/udp.hpp>
#include <cstddef>
#include <span>
#include "Endpoint.hpp"

namespace rtype::network {

/**
 * @returns @p endpoint as Asio names it.
 * @throws InvalidAddressException when the address of @p endpoint is not an
 *         IP address.
 */
[[nodiscard]] asio::ip::udp::endpoint toAsioEndpoint(const Endpoint& endpoint);

/**
 * @returns @p destination as Asio names it, once a datagram of @p payload is
 * known to be one a socket sends: the checks of IDatagramSocket::send(), in
 * one place for every socket that promises them.
 * @throws DatagramTooLargeException when @p payload holds more than
 *         MAX_DATAGRAM_SIZE bytes.
 * @throws InvalidAddressException when the address of @p destination is not
 *         an IP address.
 */
[[nodiscard]] asio::ip::udp::endpoint sendableDestination(
    const Endpoint& destination, std::span<const std::byte> payload);

}  // namespace rtype::network
