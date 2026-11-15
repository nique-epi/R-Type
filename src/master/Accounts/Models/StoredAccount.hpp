#pragma once

#include "Account.hpp"
#include "PasswordHash.hpp"

namespace rtype::master {

/** @brief An account as the database keeps it: what the master shows of it,
 * and the hash of its password, which it never shows. */
struct StoredAccount {
  network::Account account;
  network::PasswordHash passwordHash{};
};

}  // namespace rtype::master
