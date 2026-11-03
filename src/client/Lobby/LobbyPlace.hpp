#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What lies under the waiting room, which decides where BACK leads. */
enum class LobbyPlace : std::uint8_t {
  /** @brief Pushed over the connection screen: BACK closes it, back to that
   * screen. */
  AfterConnection,
  /** @brief Alone after a game: BACK opens the title in place of every
   * screen. */
  AfterGame,
};

}  // namespace rtype::client
