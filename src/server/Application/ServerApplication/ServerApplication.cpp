#include "ServerApplication.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <span>
#include <stop_token>
#include <thread>
#include <utility>
#include "DatagramSocketOpening.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConditions.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "TickLoop.hpp"

namespace rtype::server {

ServerApplication::ServerApplication(
    network::Endpoint localEndpoint, TickHandler handleTick,
    const network::NetworkConditions& conditions)
    : listeningEndpoint_(std::move(localEndpoint)),
      conditions_(conditions),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      connectionRequests_(CONNECTION_REQUEST_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY),
      sorter_(DatagramSorter::Destinations{.players = &incoming_,
                                           .connectionRequests =
                                               &connectionRequests_},
              clock_),
      discardReport_(incoming_, connectionRequests_, sorter_),
      handleTick_(std::move(handleTick)) {}

void ServerApplication::open() {
  if (socket_) {
    throw ServerAlreadyOpenException();
  }
  socket_ =
      network::openDatagramSocket(network_, listeningEndpoint_, conditions_);
  relay_.emplace(*socket_, incoming_, outgoing_);
}

void ServerApplication::close() {
  relay_.reset();
  socket_.reset();
}

void ServerApplication::run() {
  openSocket().startReceiving([this](const network::Endpoint& sender,
                                     std::span<const std::byte> datagram) {
    sorter_.sort(sender, datagram);
  });
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

void ServerApplication::admit(const network::Endpoint& player,
                              std::uint64_t sessionToken) {
  network_.post([this, admitted = player, sessionToken] {
    sorter_.admit(admitted, sessionToken);
  });
}

void ServerApplication::forget(const network::Endpoint& player,
                               std::uint64_t sessionToken) {
  network_.post([this, forgotten = player, sessionToken] {
    sorter_.forget(forgotten, sessionToken);
  });
}

void ServerApplication::queueDatagram(network::OutgoingDatagram datagram) {
  outgoing_.push(std::move(datagram));
}

network::Endpoint ServerApplication::localEndpoint() const {
  return openSocket().localEndpoint();
}

network::IDatagramSocket& ServerApplication::openSocket() {
  if (!socket_) {
    throw ServerNotOpenException();
  }
  return *socket_;
}

const network::IDatagramSocket& ServerApplication::openSocket() const {
  if (!socket_) {
    throw ServerNotOpenException();
  }
  return *socket_;
}

void ServerApplication::simulateTick() {
  received_.clear();
  incoming_.drainInto(received_);
  receivedRequests_.clear();
  connectionRequests_.drainInto(receivedRequests_);
  handleTick_(ReceivedDatagrams{.players = received_,
                                .connectionRequests = receivedRequests_});
  discardReport_.countTick();
  network_.post([this] {
    if (relay_.has_value()) {
      relay_->sendWaiting();
    }
  });
}

}  // namespace rtype::server
