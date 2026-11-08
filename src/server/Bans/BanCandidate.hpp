#pragma once

#include <chrono>
#include <string_view>

namespace rtype::server {

/** @brief A player as a ban sees it: its nickname, its address and the moment
 * it is judged at. */
struct BanCandidate {
  std::string_view nickname;
  std::string_view address;
  std::chrono::sys_seconds at;
};

}  // namespace rtype::server
