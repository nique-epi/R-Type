#pragma once

#include <chrono>
#include "Account.hpp"

namespace rtype::master {

/** @brief The account a session that has not expired belongs to, and when the
 * session expires. */
struct SessionOwner {
  network::Account account;
  std::chrono::sys_seconds expiresAt;
};

}  // namespace rtype::master
