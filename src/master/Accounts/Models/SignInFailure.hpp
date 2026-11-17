#pragma once

#include <chrono>
#include <string>

namespace rtype::master {

/** @brief A sign-in that failed: the address it came from, and when. */
struct SignInFailure {
  std::string address;
  std::chrono::sys_seconds failedAt;
};

}  // namespace rtype::master
