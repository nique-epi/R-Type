#pragma once

#include <chrono>
#include <cstddef>

namespace rtype::network {

/**
 * @brief How many datagrams a sender may send, and what happens past it.
 *
 * A sender may send burst datagrams at once after a silence, then one every
 * interval: the generic cell rate algorithm of ITU-T I.371, which keeps one
 * time per sender (see spendAllowance()). A sender dropped dropsBeforeBan
 * times within dropWindow, counted from its first drop, is banned for
 * banDuration: every datagram it sends until then is dropped. interval,
 * dropWindow and banDuration are above zero, burst and dropsBeforeBan at
 * least 1.
 */
struct RateLimit {
  std::chrono::nanoseconds interval{};
  std::size_t burst{};
  std::size_t dropsBeforeBan{};
  std::chrono::nanoseconds dropWindow{};
  std::chrono::nanoseconds banDuration{};
};

}  // namespace rtype::network
