#pragma once

#include <chrono>
#include <string>

namespace rtype::master {

/** @brief Where and when a session opens: the address it is counted under,
 * and the second it opens at. */
struct SessionOrigin {
  std::string address;
  std::chrono::sys_seconds openedAt;
};

}  // namespace rtype::master
