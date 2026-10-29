#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What a step of the help shows under its bubble: the waiting room as
 * it is, or a moment of a game the help paints itself. */
enum class HelpScene : std::uint8_t {
  Lobby,
  Game,
};

}  // namespace rtype::client
