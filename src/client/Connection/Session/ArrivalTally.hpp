#pragma once

#include <cstdint>
#include "ISessionArrivals.hpp"
#include "SessionArrivals.hpp"

namespace rtype::client {

/**
 * @brief Counts the datagrams of the server a session accepts, and the
 * sequences it skips: a datagram accepted three sequences after the newest one
 * before it means two datagrams never accepted, lost or
 * refused for arriving after a newer one. A duplicate or a late datagram is
 * refused before it reaches the tally, so it counts nothing again.
 *
 * Not thread-safe: use it from the frame loop, which reads the session.
 */
class ArrivalTally final : public ISessionArrivals {
 public:
  /** @brief Counts a datagram accepted @p distance sequences after the newest
   * accepted before it, 1 to 32767. */
  void countAccepted(std::uint16_t distance);

  [[nodiscard]] SessionArrivals arrivals() const override;

 private:
  SessionArrivals arrivals_;
};

}  // namespace rtype::client
