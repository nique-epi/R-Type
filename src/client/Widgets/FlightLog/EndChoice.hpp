#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the player chooses at the end of a game. */
enum class EndChoice : std::uint8_t {
  Replay,
  Quit,
};

}  // namespace rtype::client
