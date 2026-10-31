#pragma once

#include <asio/steady_timer.hpp>
#include <cstddef>
#include <memory>
#include <span>
#include "DatagramsInFlight.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkContext.hpp"

namespace rtype::network {

/**
 * @brief One direction of a simulated network on the event loop of a
 * NetworkContext: the datagrams in flight, and a timer that hands each to its
 * delivery when it arrives.
 *
 * Every call must run on the thread that runs the context, or before that
 * thread starts. The handler of the timer keeps the object alive, as the
 * operations of a UdpSocket keep its state, so a deadline that falls after
 * the socket is destroyed touches no freed memory. That is the only reason
 * for the base enable_shared_from_this: build it with std::make_shared.
 */
class DelayedDelivery final
    : public std::enable_shared_from_this<DelayedDelivery> {
 public:
  /**
   * @param deliver Called once per arrival, on the thread that runs
   * @p context, with bytes only valid during the call. An exception it throws
   * leaves NetworkContext::run().
   */
  DelayedDelivery(NetworkContext& context, DatagramsInFlight inFlight,
                  DatagramHandler deliver);

  /**
   * @brief Hands @p payload, for @p peer, to the simulated network: it reaches
   * the delivery once its delay ends, twice, or never. The bytes are copied.
   */
  void add(const Endpoint& peer, std::span<const std::byte> payload);

  /** @brief Stops every delivery for good: what is in flight never arrives. */
  void close();

 private:
  /** @brief Arms the timer for the next arrival, unless closed or nothing is
   * in flight. */
  void waitForNextArrival();

  /** @brief Delivers what arrived, then waits for the next arrival. */
  void deliverArrived();

  DatagramsInFlight inFlight_;
  asio::steady_timer timer_;
  DatagramHandler deliver_;
  bool closed_{false};
};

}  // namespace rtype::network
