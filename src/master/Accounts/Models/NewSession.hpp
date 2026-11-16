#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace rtype::master {

/** @brief A session to add: the fingerprint of its token, never the token, its
 * account, where it was opened from, and when it opens and expires. */
struct NewSession {
  std::string tokenFingerprint;
  std::int64_t userId{};
  std::string address;
  std::chrono::sys_seconds createdAt;
  std::chrono::sys_seconds expiresAt;
};

}  // namespace rtype::master
