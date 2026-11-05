#pragma once

#include <string_view>
#include "RouteMethod.hpp"

namespace rtype::network {

/**
 * @brief A route of the API of the master, declared once and read by the
 * master that answers it and by the client that calls it.
 */
struct MasterRoute {
  RouteMethod method{RouteMethod::Get};
  /** @brief Its path below MASTER_API_PREFIX, such as `/servers/me`. */
  std::string_view path;
};

}  // namespace rtype::network
