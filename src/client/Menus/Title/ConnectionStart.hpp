#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief When a connection screen starts its attempt. */
enum class ConnectionStart : std::uint8_t {
  /** @brief Once the player confirms the fields, which start with the server
   * the form remembers (ServerChoice::server). */
  OnConfirm,
  /** @brief Once the player confirms the fields, which start with the server
   * of the last attempt: the form RETRY opens after a lost game. */
  LastAttemptOnConfirm,
  /** @brief As soon as it is built, with the server of the last attempt, or
   * the server the form remembers before any: the server map chose it, or
   * `-h` named it. */
  AtOnce,
};

}  // namespace rtype::client
