#include "ServerApplication.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <span>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
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

namespace rtype::server {

namespace {

/**
 * @returns The end of the report line that names @p sender, or nothing when
 * there is no sender to name.
 */
std::string firstSenderText(const std::optional<network::Endpoint>& sender) {
  if (!sender.has_value()) {
    return {};
  }
  return ", the first from " + sender->address + " port " +
         std::to_string(sender->port);
}

/**
 * @returns How many events @p current counts beyond @p reported, which then
 * holds @p current: the events not yet reported.
 */
std::size_t countedSince(std::size_t current, std::size_t& reported) {
  const std::size_t fresh = current - reported;
  reported = current;
  return fresh;
}

}  // namespace

ServerApplication::ServerApplication(
    network::Endpoint localEndpoint, TickHandler handleTick,
    const network::NetworkConditions& conditions)
    : logger_(std::string{SERVER_LOGGER_NAME}),
      listeningEndpoint_(std::move(localEndpoint)),
      conditions_(conditions),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      connectionRequests_(CONNECTION_REQUEST_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY),
      sorter_(DatagramQueues{.players = &incoming_,
                             .connectionRequests = &connectionRequests_},
              clock_),
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
  reportDiscardedDatagrams();
  network_.post([this] {
    if (relay_.has_value()) {
      relay_->sendWaiting();
    }
  });
}

void ServerApplication::reportDiscardedDatagrams() {
  ++ticksSinceDiscardReport_;
  if (ticksSinceDiscardReport_ < TICKS_PER_REPORT) {
    return;
  }
  ticksSinceDiscardReport_ = 0;
  if (const std::size_t fresh =
          countedSince(incoming_.discardedCount(), reportedDiscardCount_)) {
    logger_.warn(fresh, " datagrams of players discarded in the last ",
                 REPORT_SECONDS, " s: more than ",
                 INCOMING_DATAGRAM_QUEUE_CAPACITY,
                 " arrived between two ticks");
  }
  if (const std::size_t fresh = countedSince(
          connectionRequests_.discardedCount(), reportedRequestDiscardCount_)) {
    logger_.warn(fresh, " connection requests discarded in the last ",
                 REPORT_SECONDS, " s: more than ",
                 CONNECTION_REQUEST_QUEUE_CAPACITY,
                 " arrived between two ticks");
  }
  if (const std::size_t fresh =
          countedSince(sorter_.droppedCount(), reportedDroppedCount_)) {
    logger_.warn(fresh, " datagrams dropped before any queue in the last ",
                 REPORT_SECONDS,
                 " s: not of this protocol and version, malformed, or from a "
                 "sender that was not admitted");
  }
  if (const std::size_t fresh =
          countedSince(sorter_.wrongTokenCount(), reportedWrongTokenCount_)) {
    logger_.warn(fresh, " datagrams dropped in the last ", REPORT_SECONDS,
                 " s: sent from the address of a player with another session "
                 "token",
                 firstSenderText(sorter_.takeWrongTokenSender()));
  }
}

}  // namespace rtype::server
