#include "ServerApplication.hpp"
#include <cstddef>
#include <exception>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include "Endpoint.hpp"
#include "ServerConstants.hpp"
#include "TickLoop.hpp"

namespace rtype::server {

ServerApplication::ServerApplication(const network::Endpoint& localEndpoint,
                                     TickHandler handleTick)
    : logger_(std::string{SERVER_LOGGER_NAME}),
      socket_(network_, localEndpoint),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY),
      relay_(socket_, incoming_, outgoing_),
      handleTick_(std::move(handleTick)) {}

void ServerApplication::run() {
  relay_.startReceiving();
  TickLoop simulation{clock_, [this] { simulateTick(); }};
  std::exception_ptr simulationError;
  {
    const std::jthread simulationThread{
        [this, &simulation, &simulationError](const std::stop_token& stop) {
          try {
            simulation.run(stop);
          } catch (...) {
            simulationError = std::current_exception();
            network_.stop();
          }
        }};
    network_.run();
  }
  if (simulationError) {
    std::rethrow_exception(simulationError);
  }
}

void ServerApplication::stop() { network_.stop(); }

network::Endpoint ServerApplication::localEndpoint() const {
  return socket_.localEndpoint();
}

void ServerApplication::simulateTick() {
  received_.clear();
  incoming_.drainInto(received_);
  handleTick_(received_);
  reportDiscardedDatagrams();
  network_.post([this] { relay_.sendWaiting(); });
}

void ServerApplication::reportDiscardedDatagrams() {
  const std::size_t discardCount = incoming_.discardedCount();
  if (discardCount == reportedDiscardCount_) {
    return;
  }
  logger_.warn(discardCount - reportedDiscardCount_,
               " received datagrams discarded: the simulation fell behind");
  reportedDiscardCount_ = discardCount;
}

}  // namespace rtype::server
