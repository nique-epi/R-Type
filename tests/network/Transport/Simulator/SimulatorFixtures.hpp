#pragma once

#include <cstddef>
#include <vector>
#include "Endpoint.hpp"

/** @returns The peer of the tests: PEER_PORT on the loopback address. */
[[nodiscard]] rtype::network::Endpoint peer();

/** @returns CROSSING_BYTES, as the payload of a datagram. */
[[nodiscard]] std::vector<std::byte> crossingBytes();
