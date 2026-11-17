#pragma once

#include "PasswordHash.hpp"
#include "User.hpp"

namespace rtype::master {

/** @brief An account as the database keeps it: what the master shows of it,
 * and the hash of its password, which it never shows. */
struct StoredUser {
  network::User user;
  network::PasswordHash passwordHash{};
};

}  // namespace rtype::master
