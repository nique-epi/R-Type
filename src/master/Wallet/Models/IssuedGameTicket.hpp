#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace rtype::master {

/** @brief A game ticket as the master keeps it: its fingerprint, never the
 * ticket, the account it was given to, and its lifetime. */
struct IssuedGameTicket {
  std::string fingerprint;
  std::int64_t userId{};
  std::chrono::sys_seconds issuedAt;
  std::chrono::sys_seconds expiresAt;
};

}  // namespace rtype::master
