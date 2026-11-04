#pragma once

#include <cstddef>
#include <vector>
#include "RejectedSender.hpp"
#include "RejectionCounts.hpp"

namespace rtype::server {

/**
 * @brief What a RejectionLedger counted, as RejectionLedger::takeReport() hands
 * it over.
 */
struct RejectionReport {
  /** @brief The datagrams rejected since the previous report, by cause. */
  RejectionCounts sincePreviousReport{};
  /** @brief How many datagrams were rejected since the ledger was built. */
  std::size_t rejectedSinceStart{0};
  /**
   * @brief At most REPORTED_SENDER_COUNT senders rejected since the previous
   * report: the most rejected first, and among those rejected as often, the
   * one rejected most recently first.
   */
  std::vector<RejectedSender> mostRejected;
  /** @brief How many other senders the ledger held. */
  std::size_t otherSenders{0};
  /**
   * @brief How many senders the ledger forgot since the previous report to
   * make room for a new one: their datagrams count in sincePreviousReport, in
   * no sender.
   */
  std::size_t forgottenSenders{0};
};

}  // namespace rtype::server
