#include "DiscardReport.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "BoundedQueue.hpp"
#include "DatagramDrops.hpp"
#include "DatagramQueues.hpp"
#include "Endpoint.hpp"
#include "Logger.hpp"
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

/** @brief What a queue holds, as its line names it, and its room. */
struct DiscardedQueue {
  std::string_view held;
  std::size_t capacity{};
};

/** @brief Warns of the @p fresh datagrams @p queue discarded since the
 * previous report, when there are any. */
void warnDiscards(const logging::Logger& logger, std::size_t fresh,
                  const DiscardedQueue& queue) {
  if (fresh > 0) {
    logger.warn(fresh, " ", queue.held, " discarded in the last ",
                REPORT_SECONDS, " s: more than ", queue.capacity,
                " arrived between two ticks");
  }
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
  warnDiscards(
      logger_,
      countedSince(queues_.players->discardedCount(), reportedDiscardCount_),
      DiscardedQueue{.held = "datagrams of players",
                     .capacity = INCOMING_DATAGRAM_QUEUE_CAPACITY});
  warnDiscards(logger_,
               countedSince(queues_.connectionRequests->discardedCount(),
                            reportedRequestDiscardCount_),
               DiscardedQueue{.held = "connection requests",
                              .capacity = CONNECTION_REQUEST_QUEUE_CAPACITY});
  warnDiscards(logger_,
               countedSince(queues_.informationRequests->discardedCount(),
                            reportedInformationDiscardCount_),
               DiscardedQueue{.held = "information requests",
                              .capacity = INFORMATION_REQUEST_QUEUE_CAPACITY});
  reportDrops();
}

void DiscardReport::reportDrops() {
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
