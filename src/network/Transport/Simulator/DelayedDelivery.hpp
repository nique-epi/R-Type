#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include "DatagramsInFlight.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkContext.hpp"

namespace rtype::network {

struct DelayedDeliveryState;

/**
 * @brief One direction of a simulated network on the event loop of a
 * NetworkContext: the datagrams in flight, and a timer that hands each to its
 * delivery when it arrives.
 *
 * While a datagram is in flight and the object is not closed, the timer waits
 * for the earliest arrival; it is armed again only when that arrival changes,
 * so a datagram that arrives after the earliest one costs no new wait.
 *
 * Every call must run on the thread that runs the context, or before that
 * thread starts. Its state is a DelayedDeliveryState it shares with the
 * handler of the timer, as the operations of a UdpSocket share its state, so a
 * deadline that falls after the object is destroyed touches no freed memory
 * and, the object being closed by then, delivers nothing.
 */
class DelayedDelivery final {
 public:
  /**
   * @param deliver Called once per arrival, on the thread that runs
   * @p context, with bytes only valid during the call. An exception it throws
   * leaves NetworkContext::run().
   */
  DelayedDelivery(NetworkContext& context, DatagramsInFlight inFlight,
                  DatagramHandler deliver);

  /** @brief Closes it, so a deadline that falls later delivers nothing. */
  ~DelayedDelivery();

  DelayedDelivery(const DelayedDelivery&) = delete;
  DelayedDelivery& operator=(const DelayedDelivery&) = delete;
  DelayedDelivery(DelayedDelivery&&) = delete;
  DelayedDelivery& operator=(DelayedDelivery&&) = delete;

  /**
   * @brief Hands @p payload, for @p peer, to the simulated network: it reaches
   * the delivery once its delay ends, twice, or never. The bytes are copied.
   */
  void add(const Endpoint& peer, std::span<const std::byte> payload);

  /**
   * @brief Stops every delivery for good: what is in flight never arrives.
   *
   * The timer is left to its deadline, whose handler then delivers nothing:
   * cancelling it may throw, since Asio is built with ASIO_NO_DEPRECATED,
   * which leaves only the throwing steady_timer::cancel(), and the destructors
   * of this class and of SimulatedNetworkSocket call this.
   */
  void close() noexcept;

 private:
  std::shared_ptr<DelayedDeliveryState> state_;
};

}  // namespace rtype::network
