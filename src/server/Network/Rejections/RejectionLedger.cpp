#include "RejectionLedger.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
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

}  // namespace

RejectionLedger::RejectionLedger() {
  senders_.reserve(REJECTED_SENDER_CAPACITY);
}

void RejectionLedger::record(RejectionCause cause,
                             const network::Endpoint& sender) {
  const std::scoped_lock lock{mutex_};
  addOneTo(sincePreviousReport_, cause);
  addOneTo(sinceStart_, cause);
  RejectedSender& place = placeOf(sender);
  addOneTo(place.counts, cause);
  place.latestRejection = ++recordedCount_;
}

RejectionReport RejectionLedger::takeReport() {
  const std::scoped_lock lock{mutex_};
  std::vector<RejectedSender> named = mostRejected();
  const std::size_t otherSenders = senders_.size() - named.size();
  senders_.clear();
  return RejectionReport{
      .sincePreviousReport = std::exchange(sincePreviousReport_, {}),
      .sinceStart = sinceStart_,
      .mostRejected = std::move(named),
      .otherSenders = otherSenders,
      .forgottenSenders = std::exchange(forgottenSenders_, 0)};
}

RejectedSender& RejectionLedger::placeOf(const network::Endpoint& sender) {
  const auto known =
      std::ranges::find(senders_, sender, &RejectedSender::endpoint);
  if (known != senders_.end()) {
    return *known;
  }
  if (senders_.size() < REJECTED_SENDER_CAPACITY) {
    return senders_.emplace_back(RejectedSender{.endpoint = sender});
  }
  RejectedSender& leastRecent =
      *std::ranges::min_element(senders_, {}, &RejectedSender::latestRejection);
  leastRecent = RejectedSender{.endpoint = sender};
  ++forgottenSenders_;
  return leastRecent;
}

std::vector<RejectedSender> RejectionLedger::mostRejected() const {
  std::vector<RejectedSender> sorted = senders_;
  const auto namedCount = static_cast<std::ptrdiff_t>(
      std::min(sorted.size(), REPORTED_SENDER_COUNT));
  std::ranges::partial_sort(sorted, std::next(sorted.begin(), namedCount),
                            isNamedBefore);
  sorted.erase(std::next(sorted.begin(), namedCount), sorted.end());
  return sorted;
}

}  // namespace rtype::server
