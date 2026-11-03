#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What an accepted connection leads to. */
enum class ConnectionGoal : std::uint8_t {
  /** @brief Opens the waiting room over the connection screen. */
  WaitingRoom,
  /** @brief Closes the connection screen: the screen below it, which sent the
   * player here, reads the session. */
  ScreenBelow,
};

}  // namespace rtype::client
