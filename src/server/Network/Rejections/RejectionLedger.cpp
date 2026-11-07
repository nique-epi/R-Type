#include "RejectionLedger.hpp"
#include <algorithm>
#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "RejectedSender.hpp"
#include "RejectionCause.hpp"
#include "RejectionCounts.hpp"
#include "RejectionReport.hpp"
#include "RejectionsConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns Whether a report names @p first before @p second: it was rejected
 * more often, or as often and more recently.
 */
bool isNamedBefore(const RejectedSender& first, const RejectedSender& second) {
  const std::size_t firstTotal = totalOf(first.counts);
  const std::size_t secondTotal = totalOf(second.counts);
  if (firstTotal != secondTotal) {
    return firstTotal > secondTotal;
  }
  return first.latestRejection > second.latestRejection;
}

/**
 * @returns Whether @p place holds @p sender. The port is compared first: forged
 * addresses often share the length of their text, and rarely their port.
 */
bool holds(const RejectedSender& place, const network::Endpoint& sender) {
  return place.endpoint.port == sender.port &&
         place.endpoint.address == sender.address;
}

/**
 * @brief Gives @p place to @p sender, counts at 0, reusing the room of the
 * address it held.
 */
void handOver(RejectedSender& place, const network::Endpoint& sender) {
  place.endpoint.address.assign(sender.address);
  place.endpoint.port = sender.port;
  place.counts = {};
}

}  // namespace

RejectionLedger::RejectionLedger() {
  senders_.reserve(REJECTED_SENDER_CAPACITY);
}

void RejectionLedger::record(RejectionCause cause,
                             const network::Endpoint& sender) {
  const std::scoped_lock lock{mutex_};
  addOneTo(sincePreviousReport_, cause);
  RejectedSender& place = placeOf(sender);
  addOneTo(place.counts, cause);
  place.latestRejection = ++rejectedSinceStart_;
}

RejectionReport RejectionLedger::takeReport() {
  const std::scoped_lock lock{mutex_};
  std::vector<RejectedSender> named(
      std::min(senders_.size(), REPORTED_SENDER_COUNT));
  std::ranges::partial_sort_copy(senders_, named, isNamedBefore);
  const std::size_t otherSenders = senders_.size() - named.size();
  senders_.clear();
  return RejectionReport{
      .sincePreviousReport = std::exchange(sincePreviousReport_, {}),
      .rejectedSinceStart = rejectedSinceStart_,
      .mostRejected = std::move(named),
      .otherSenders = otherSenders,
      .forgottenSenders = std::exchange(forgottenSenders_, 0)};
}

std::size_t RejectionLedger::rejectedSinceStart() const {
  const std::scoped_lock lock{mutex_};
  return rejectedSinceStart_;
}

RejectedSender& RejectionLedger::placeOf(const network::Endpoint& sender) {
  std::size_t leastRecent = 0;
  for (std::size_t index = 0; index < senders_.size(); ++index) {
    if (holds(senders_[index], sender)) {
      return senders_[index];
    }
    if (senders_[index].latestRejection <
        senders_[leastRecent].latestRejection) {
      leastRecent = index;
    }
  }
  if (senders_.size() < REJECTED_SENDER_CAPACITY) {
    return senders_.emplace_back(RejectedSender{.endpoint = sender});
  }
  handOver(senders_[leastRecent], sender);
  ++forgottenSenders_;
  return senders_[leastRecent];
}

}  // namespace rtype::server
