#pragma once

#include <asio/steady_timer.hpp>
#include "DatagramsInFlight.hpp"
#include "IDatagramSocket.hpp"

namespace rtype::network {

/**
 * @brief What a DelayedDelivery shares with the handler of its timer: the
 * datagrams in flight, the timer that waits for the next arrival, the delivery
 * and whether it is closed.
 *
 * The handler holds it, so a deadline that falls after the DelayedDelivery is
 * destroyed touches no freed memory.
 */
struct DelayedDeliveryState {
  DatagramsInFlight inFlight;
  asio::steady_timer timer;
  DatagramHandler deliver;
  bool closed{false};
};

}  // namespace rtype::network
