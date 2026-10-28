#pragma once

#include <cstdint>

namespace rtype::tools {

/**
 * @brief Who the tool floods: a server it joins as a player, or a client that
 * takes the tool for its server.
 */
enum class FloodMode : std::uint8_t {
  Server,
  Client,
};

}  // namespace rtype::tools
