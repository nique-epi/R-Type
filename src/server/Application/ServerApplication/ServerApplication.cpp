#include "ServerApplication.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <optional>
#include <span>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerConstants.hpp"
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

}  // namespace

ServerApplication::ServerApplication(const network::Endpoint& localEndpoint,
                                     TickHandler handleTick)
    : logger_(std::string{SERVER_LOGGER_NAME}),
      socket_(network_, localEndpoint),
      incoming_(INCOMING_DATAGRAM_QUEUE_CAPACITY),
      connectionRequests_(CONNECTION_REQUEST_QUEUE_CAPACITY),
      outgoing_(OUTGOING_DATAGRAM_QUEUE_CAPACITY),
      sorter_(DatagramSorter::Destinations{
          .players = &incoming_, .connectionRequests = &connectionRequests_}),
      relay_(socket_, incoming_, outgoing_),
      handleTick_(std::move(handleTick)) {}

void ServerApplication::run() {
  socket_.startReceiving([this](const network::Endpoint& sender,
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

void ServerApplication::queueDatagram(network::OutgoingDatagram datagram) {
  outgoing_.push(std::move(datagram));
}

network::Endpoint ServerApplication::localEndpoint() const {
  return socket_.localEndpoint();
}

void ServerApplication::simulateTick() {
  received_.clear();
  incoming_.drainInto(received_);
  receivedRequests_.clear();
  connectionRequests_.drainInto(receivedRequests_);
  handleTick_(ReceivedDatagrams{.players = received_,
                                .connectionRequests = receivedRequests_});
  reportDiscardedDatagrams();
  network_.post([this] { relay_.sendWaiting(); });
}

void ServerApplication::reportDiscardedDatagrams() {
  ++ticksSinceDiscardReport_;
  if (ticksSinceDiscardReport_ < TICKS_PER_REPORT) {
    return;
  }
  ticksSinceDiscardReport_ = 0;
  const std::size_t discardCount = incoming_.discardedCount();
  if (discardCount != reportedDiscardCount_) {
    logger_.warn(discardCount - reportedDiscardCount_,
                 " datagrams of players discarded in the last ", REPORT_SECONDS,
                 " s: more than ", INCOMING_DATAGRAM_QUEUE_CAPACITY,
                 " arrived between two ticks");
    reportedDiscardCount_ = discardCount;
  }
  const std::size_t requestDiscardCount = connectionRequests_.discardedCount();
  if (requestDiscardCount != reportedRequestDiscardCount_) {
    logger_.warn(requestDiscardCount - reportedRequestDiscardCount_,
                 " connection requests discarded in the last ", REPORT_SECONDS,
                 " s: more than ", CONNECTION_REQUEST_QUEUE_CAPACITY,
                 " arrived between two ticks");
    reportedRequestDiscardCount_ = requestDiscardCount;
  }
  const std::size_t droppedCount = sorter_.droppedCount();
  if (droppedCount != reportedDroppedCount_) {
    logger_.warn(droppedCount - reportedDroppedCount_,
                 " datagrams dropped before any queue in the last ",
                 REPORT_SECONDS,
                 " s: not of this protocol and version, malformed, or from a "
                 "sender that was not admitted");
    reportedDroppedCount_ = droppedCount;
  }
  const std::size_t wrongTokenCount = sorter_.wrongTokenCount();
  if (wrongTokenCount != reportedWrongTokenCount_) {
    logger_.warn(wrongTokenCount - reportedWrongTokenCount_,
                 " datagrams dropped in the last ", REPORT_SECONDS,
                 " s: sent from the address of a player with another session "
                 "token",
                 firstSenderText(sorter_.takeWrongTokenSender()));
    reportedWrongTokenCount_ = wrongTokenCount;
  }
}

}  // namespace rtype::server
