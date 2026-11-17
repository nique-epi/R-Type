#pragma once

#include <chrono>
#include <string>

namespace rtype::master {

/** @brief A device link to add: its short code, the fingerprint of its secret,
 * never the secret, the address that asked for it, and when it opens and
 * expires. */
struct NewDeviceLink {
  std::string userCode;
  std::string secretFingerprint;
  std::string address;
  std::chrono::sys_seconds createdAt;
  std::chrono::sys_seconds expiresAt;
};

}  // namespace rtype::master
