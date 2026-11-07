#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief When a connection screen starts its attempt. */
enum class ConnectionStart : std::uint8_t {
  /** @brief Once the player confirms the fields. */
  OnConfirm,
  /** @brief As soon as it is built, with the server and the nickname of the
   * ServerChoice: the server map chose them, or `-h` named the server. */
  AtOnce,
};

}  // namespace rtype::client
