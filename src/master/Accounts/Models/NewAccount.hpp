#pragma once

#include <chrono>
#include <string>
#include "PasswordHash.hpp"

namespace rtype::master {

/** @brief An account to add: its nickname, the hash of its password, and who
 * created it when. It is a player until an `admin` line names it. */
struct NewAccount {
  std::string nickname;
  network::PasswordHash passwordHash{};
  std::string createdByAddress;
  std::chrono::sys_seconds createdAt;
};

}  // namespace rtype::master
