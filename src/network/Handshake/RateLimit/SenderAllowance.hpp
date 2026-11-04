#pragma once

#include <chrono>
#include <cstddef>
#include <optional>

namespace rtype::network {

/**
 * @brief What one sender spent of its RateLimit. A default one has sent
 * nothing yet. Every time is measured from the origin of the clock the caller
 * reads.
 *
 * - theoreticalArrival: when its next datagram is due if it had kept to the
 *   interval since its first one.
 * - dropWindowEnd: the end of the window of drops its first drop opened.
 * - dropsInWindow: its datagrams dropped in that window.
 * - bannedUntil: the end of its last ban.
 */
struct SenderAllowance {
  std::optional<std::chrono::nanoseconds> theoreticalArrival;
  std::optional<std::chrono::nanoseconds> dropWindowEnd;
  std::size_t dropsInWindow{};
  std::optional<std::chrono::nanoseconds> bannedUntil;
};

}  // namespace rtype::network
