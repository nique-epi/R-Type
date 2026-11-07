#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::network {

/**
 * @brief The windows a CookieIssuer computes its cookies in: how long one
 * lasts, and how many it accepts, the window a cookie was issued in included.
 */
struct CookieWindows {
  std::chrono::nanoseconds duration{};
  std::uint64_t accepted{};
};

}  // namespace rtype::network
