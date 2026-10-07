#include "ServerApplication.hpp"
#include <cstddef>
#include <exception>
#include <stop_token>
#include <string>
#include <thread>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

ServerApplication::ServerApplication(const network::Endpoint& localEndpoint)
    : logger_(std::string{SERVER_LOGGER_NAME}),
      socket_(network_, localEndpoint),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY),
      relay_(socket_, incoming_, outgoing_),
      simulation_(clock_, [this] { simulateTick(); }) {
  world_.spawnPlayer();
}

void ServerApplication::run() {
  relay_.startReceiving();
  std::exception_ptr simulationError;
  {
    const std::jthread simulationThread{
        [this, &simulationError](const std::stop_token& stop) {
          try {
            simulation_.run(stop);
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
  for (const network::IncomingDatagram& datagram : received_) {
    logger_.debug("received ", datagram.payload.size(), " bytes from ",
                  datagram.sender.address, " port ", datagram.sender.port);
  }
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
