#include "DelayedDelivery.hpp"
#include <asio/error.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>
#include <cstddef>
#include <optional>
#include <span>
#include <system_error>
#include <utility>
#include "DatagramInFlight.hpp"
#include "DatagramsInFlight.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkContext.hpp"

namespace rtype::network {

DelayedDelivery::DelayedDelivery(NetworkContext& context,
                                 DatagramsInFlight inFlight,
                                 DatagramHandler deliver)
    : inFlight_(std::move(inFlight)),
      timer_(context.ioContext()),
      deliver_(std::move(deliver)) {}

void DelayedDelivery::add(const Endpoint& peer,
                          std::span<const std::byte> payload) {
  const std::optional<std::chrono::steady_clock::time_point> previousNext =
      inFlight_.nextArrival();
  inFlight_.add(peer, payload, std::chrono::steady_clock::now());
  if (inFlight_.nextArrival() != previousNext) {
    waitForNextArrival();
  }
}

void DelayedDelivery::close() {
  closed_ = true;
  timer_.cancel();
}

void DelayedDelivery::waitForNextArrival() {
  const std::optional<std::chrono::steady_clock::time_point> next =
      inFlight_.nextArrival();
  if (closed_ || !next.has_value()) {
    return;
  }
  timer_.expires_at(*next);
  timer_.async_wait([self = shared_from_this()](const std::error_code& error) {
    if (error != asio::error::operation_aborted) {
      self->deliverArrived();
    }
  });
}

void DelayedDelivery::deliverArrived() {
  for (const DatagramInFlight& datagram :
       inFlight_.takeArrived(std::chrono::steady_clock::now())) {
    if (closed_) {
      return;
    }
    deliver_(datagram.peer, datagram.payload);
  }
  waitForNextArrival();
}

}  // namespace rtype::network
