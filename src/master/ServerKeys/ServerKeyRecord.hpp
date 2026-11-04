#pragma once

#include <chrono>
#include <string>

namespace rtype::master {

/** @brief A key an admin issued, as the table of keys keeps it. */
struct ServerKeyRecord {
  /** @brief The name the admin gave it, such as the server it is for. */
  std::string name;
  /** @brief Its fingerprintOf(), never the key itself. */
  std::string fingerprint;
  std::chrono::sys_seconds issuedAt;
};

}  // namespace rtype::master
