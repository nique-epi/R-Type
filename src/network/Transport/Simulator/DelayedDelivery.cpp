#include "DelayedDelivery.hpp"
#include <asio/error.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <system_error>
#include <utility>
#include "DatagramInFlight.hpp"
#include "DatagramsInFlight.hpp"
#include "DelayedDeliveryState.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkContext.hpp"

namespace rtype::network {

namespace {

void deliverArrived(const std::shared_ptr<DelayedDeliveryState>& state);

/** @brief Arms the timer of @p state for the next arrival, unless it is closed
 * or nothing is in flight; the handler of the timer holds @p state. */
void waitForNextArrival(const std::shared_ptr<DelayedDeliveryState>& state) {
  const std::optional<std::chrono::steady_clock::time_point> next =
      state->inFlight.nextArrival();
  if (state->closed || !next.has_value()) {
    return;
  }
  state->timer.expires_at(*next);
  state->timer.async_wait([state](const std::error_code& error) {
    if (error != asio::error::operation_aborted) {
      deliverArrived(state);
    }
  });
}

/** @brief Delivers what arrived in @p state, then waits for the next
 * arrival. */
void deliverArrived(const std::shared_ptr<DelayedDeliveryState>& state) {
  for (const DatagramInFlight& datagram :
       state->inFlight.takeArrived(std::chrono::steady_clock::now())) {
    if (state->closed) {
      return;
    }
    state->deliver(datagram.peer, datagram.payload);
  }
  waitForNextArrival(state);
}

}  // namespace

DelayedDelivery::DelayedDelivery(NetworkContext& context,
                                 DatagramsInFlight inFlight,
                                 DatagramHandler deliver)
    : state_(std::make_shared<DelayedDeliveryState>(
          DelayedDeliveryState{.inFlight = std::move(inFlight),
                               .timer = asio::steady_timer{context.ioContext()},
                               .deliver = std::move(deliver)})) {}

DelayedDelivery::~DelayedDelivery() { close(); }

void DelayedDelivery::add(const Endpoint& peer,
                          std::span<const std::byte> payload) {
  const std::optional<std::chrono::steady_clock::time_point> previousNext =
      state_->inFlight.nextArrival();
  state_->inFlight.add(peer, payload, std::chrono::steady_clock::now());
  if (state_->inFlight.nextArrival() != previousNext) {
    waitForNextArrival(state_);
  }
}

void DelayedDelivery::close() noexcept { state_->closed = true; }

}  // namespace rtype::network
