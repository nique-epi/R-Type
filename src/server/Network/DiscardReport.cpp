#include "DiscardReport.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include "BoundedQueue.hpp"
#include "DatagramDrops.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "ServerConstants.hpp"

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

DiscardReport::DiscardReport(DatagramQueues queues, DatagramDrops& drops)
    : logger_(std::string{SERVER_LOGGER_NAME}),
      queues_(queues),
      drops_(&drops) {}

void DiscardReport::countTick() {
  ++ticksSinceReport_;
  if (ticksSinceReport_ < TICKS_PER_REPORT) {
    return;
  }
  ticksSinceReport_ = 0;
  report();
}

void DiscardReport::report() {
  if (const std::size_t fresh = countedSince(queues_.players->discardedCount(),
                                             reportedDiscardCount_)) {
    logger_.warn(fresh, " datagrams of players discarded in the last ",
                 REPORT_SECONDS, " s: more than ",
                 INCOMING_DATAGRAM_QUEUE_CAPACITY,
                 " arrived between two ticks");
  }
  if (const std::size_t fresh =
          countedSince(queues_.connectionRequests->discardedCount(),
                       reportedRequestDiscardCount_)) {
    logger_.warn(fresh, " connection requests discarded in the last ",
                 REPORT_SECONDS, " s: more than ",
                 CONNECTION_REQUEST_QUEUE_CAPACITY,
                 " arrived between two ticks");
  }
  if (const std::size_t fresh =
          countedSince(drops_->droppedCount(), reportedDroppedCount_)) {
    logger_.warn(fresh, " datagrams dropped before any queue in the last ",
                 REPORT_SECONDS,
                 " s: not of this protocol and version, malformed, from a "
                 "sender that was not admitted, or over the rate limit of "
                 "its sender");
  }
  if (const std::size_t fresh =
          countedSince(drops_->wrongTokenCount(), reportedWrongTokenCount_)) {
    logger_.warn(fresh, " datagrams dropped in the last ", REPORT_SECONDS,
                 " s: sent from the address of a player with another session "
                 "token",
                 firstSenderText(drops_->takeWrongTokenSender()));
  }
}

}  // namespace rtype::server
