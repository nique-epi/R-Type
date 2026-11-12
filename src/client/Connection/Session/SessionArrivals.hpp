#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The datagrams of the server the sessions of this client accepted,
 * and those missing between them, in totals that only grow. */
struct SessionArrivals {
  /** @brief Every datagram of a session accepted after its acceptance. */
  std::uint64_t accepted{0};
  /** @brief Every sequence skipped between two accepted datagrams: lost on
   * the way, or arrived after a newer one and refused. */
  std::uint64_t missed{0};

  bool operator==(const SessionArrivals&) const = default;
};

}  // namespace rtype::client
