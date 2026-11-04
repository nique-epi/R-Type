#include "ServerApplication.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <span>
#include <stop_token>
#include <thread>
#include <utility>
#include "CountingDatagramSocket.hpp"
#include "DatagramQueues.hpp"
#include "DatagramSocketOpening.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConditions.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "TickLoop.hpp"
#include "TrafficCounters.hpp"

namespace rtype::server {

namespace {

/**
 * @returns What @p socket holds.
 * @throws ServerNotOpenException when it holds nothing: open() was not called.
 */
network::IDatagramSocket& requireOpen(
    const std::unique_ptr<network::IDatagramSocket>& socket) {
  if (!socket) {
    throw ServerNotOpenException();
  }
  return *socket;
}

}  // namespace

ServerApplication::ServerApplication(
    network::Endpoint localEndpoint, TickHandler handleTick,
    const network::NetworkConditions& conditions)
    : listeningEndpoint_(std::move(localEndpoint)),
      conditions_(conditions),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      connectionRequests_(CONNECTION_REQUEST_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY),
      destinations_{.players = &incoming_,
                    .connectionRequests = &connectionRequests_},
      drops_(clock_),
      sorter_(destinations_, drops_),
      discardReport_(destinations_, drops_),
      handleTick_(std::move(handleTick)) {}

void ServerApplication::listenOn(network::Endpoint localEndpoint) {
  if (socket_) {
    throw ServerAlreadyOpenException();
  }
  listeningEndpoint_ = std::move(localEndpoint);
}

network::Endpoint ServerApplication::open() {
  if (socket_) {
    throw ServerAlreadyOpenException();
  }
  socket_ = std::make_unique<network::CountingDatagramSocket>(
      network::openDatagramSocket(network_, listeningEndpoint_, conditions_),
      traffic_);
  relay_.emplace(*socket_, incoming_, outgoing_);
  return socket_->localEndpoint();
}

void ServerApplication::close() {
  relay_.reset();
  socket_.reset();
}

void ServerApplication::run() {
  requireOpen(socket_).startReceiving(
      [this](const network::Endpoint& sender,
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
    traffic_.follow(admitted);
  });
}

void ServerApplication::forget(const network::Endpoint& player,
                               std::uint64_t sessionToken) {
  network_.post([this, forgotten = player, sessionToken] {
    if (sorter_.forget(forgotten, sessionToken)) {
      traffic_.unfollow(forgotten);
    }
  });
}

void ServerApplication::queueDatagram(network::OutgoingDatagram datagram) {
  outgoing_.push(std::move(datagram));
}

network::TrafficCounters& ServerApplication::traffic() { return traffic_; }

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
