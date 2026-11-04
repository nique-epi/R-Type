#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief The HTTP method of a route of the master. */
enum class RouteMethod : std::uint8_t {
  Get,
  Post,
  Put,
};

}  // namespace rtype::network
