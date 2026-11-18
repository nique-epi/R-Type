#pragma once

#include <chrono>

namespace rtype::network {

/** @brief What the master tells the account of a game ticket it was given:
 * when it expires, and whether a result of a game already paid through it. */
struct GameTicketState {
  std::chrono::sys_seconds expiresAt;
  bool used{false};

  bool operator==(const GameTicketState&) const = default;
};

}  // namespace rtype::network
