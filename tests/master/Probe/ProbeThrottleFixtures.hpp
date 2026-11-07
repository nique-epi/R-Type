#pragma once

#include <cstdint>
#include "Endpoint.hpp"

/** @returns THROTTLED_ADDRESS at @p port. */
[[nodiscard]] rtype::network::Endpoint throttledDestination(std::uint16_t port);
