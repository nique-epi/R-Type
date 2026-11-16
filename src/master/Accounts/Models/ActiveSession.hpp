#pragma once

#include <chrono>
#include "User.hpp"

namespace rtype::master {

/** @brief The account a session that has not expired belongs to, and when the
 * session expires. */
struct ActiveSession {
  network::User user;
  std::chrono::sys_seconds expiresAt;
};

}  // namespace rtype::master
