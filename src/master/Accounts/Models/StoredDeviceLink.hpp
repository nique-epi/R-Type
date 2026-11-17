#pragma once

#include <chrono>
#include <optional>
#include <string>
#include "DeviceLinkDecision.hpp"
#include "User.hpp"

namespace rtype::master {

/**
 * @brief A device link as the master keeps it: its short code, when it opened
 * and expires, when the game last read it, and, once a player decided, who
 * decided and what.
 */
struct StoredDeviceLink {
  std::string userCode;
  std::chrono::sys_seconds createdAt;
  std::chrono::sys_seconds expiresAt;
  std::chrono::sys_seconds readAt;
  std::optional<network::User> decider;
  std::optional<network::DeviceLinkDecision> decision;
};

}  // namespace rtype::master
