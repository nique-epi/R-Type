#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Endpoint.hpp"

/** @returns The loopback address on @p port. */
[[nodiscard]] rtype::network::Endpoint loopbackOnPort(std::uint16_t port);

/** @returns The peer of the tests: PEER_PORT on the loopback address. */
[[nodiscard]] rtype::network::Endpoint peer();

/** @returns CROSSING_BYTES, as the payload of a datagram. */
[[nodiscard]] std::vector<std::byte> crossingBytes();
