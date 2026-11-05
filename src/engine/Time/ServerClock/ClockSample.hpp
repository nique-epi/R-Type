#pragma once

#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief What one message of the server tells the client about the clock of
 * the server: the tick the server was at when it sent it, the time it arrived
 * on the clock of the client, and the round trip of the exchange it belongs
 * to.
 */
struct ClockSample {
  /** @brief When the message arrived, on the clock of the client. */
  Duration receivedAt{};
  /** @brief The round trip of the exchange, from 0 up: half of it is taken as
   * the time the message spent on its way. */
  Duration roundTrip{};
  /** @brief The tick the server was at when it sent the message. */
  std::uint32_t tick{};
};

}  // namespace rtype::engine
