#pragma once

#include <chrono>
#include <string>
#include "User.hpp"

namespace rtype::network {

/**
 * @brief What the master answers when it opened a session: its token, which it
 * never shows again, the second it expires, and the account it belongs to.
 */
struct CreatedSession {
  /** @brief Sent as `Authorization: Bearer <token>`, SESSION_TOKEN_SIZE long.
   */
  std::string token;
  std::chrono::sys_seconds expiresAt;
  User user;

  bool operator==(const CreatedSession&) const = default;
};

}  // namespace rtype::network
