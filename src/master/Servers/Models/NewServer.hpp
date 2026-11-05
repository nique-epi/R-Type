#pragma once

#include <chrono>
#include <string>

namespace rtype::master {

/** @brief A server as its creation writes it in the table `servers`. */
struct NewServer {
  std::string name;
  /** @brief The fingerprint of its key, never the key itself. */
  std::string keyFingerprint;
  /** @brief The address the request of its creation came from. */
  std::string createdByAddress;
  std::chrono::sys_seconds createdAt;
};

}  // namespace rtype::master
