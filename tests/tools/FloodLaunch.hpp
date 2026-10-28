#pragma once

#include <cstdint>
#include <optional>
#include "FloodMode.hpp"

namespace rtype::tools {

/**
 * @brief What the arguments of the tool ask: whom to flood and on which port
 * of the loopback address, or the usage.
 */
struct FloodLaunch {
  /** @brief Nothing when neither SERVER_OPTION nor CLIENT_OPTION was given. */
  std::optional<FloodMode> mode;
  std::uint16_t port{};
  bool helpRequested{false};
};

}  // namespace rtype::tools
