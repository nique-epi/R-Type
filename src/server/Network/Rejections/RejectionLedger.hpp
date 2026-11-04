#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>
#include "Endpoint.hpp"
#include "RejectedSender.hpp"
#include "RejectionCause.hpp"
#include "RejectionCounts.hpp"
#include "RejectionReport.hpp"

namespace rtype::server {

/**
 * @brief Counts the datagrams the network thread rejects, for each cause and
 * for each sender, so the server can report who sends what it refuses.
 *
 * The counts of each cause go on for the life of the ledger; those of the
 * senders start afresh at every report. The ledger keeps at most
 * REJECTED_SENDER_CAPACITY senders: a new one then takes the place of the
 * sender rejected least recently, whose counts are forgotten, and the report
 * says how many were. So a flood from forged addresses never makes the ledger
 * hold more, whatever it sends; such a flood can push a real sender out of the
 * list, but never out of the counts of its cause. The room is reserved when the
 * ledger is built: record() allocates nothing, except the text of an address
 * too long for a string to keep in itself.
 *
 * Safe from any thread: the lock is held to count one datagram or to take the
 * report.
 */
class RejectionLedger {
 public:
  RejectionLedger();

  /** @brief Counts one datagram from @p sender, rejected for @p cause. */
  void record(RejectionCause cause, const network::Endpoint& sender);

  /**
   * @returns What was counted, see RejectionReport; the senders then start
   * afresh, the counts of each cause go on.
   */
  [[nodiscard]] RejectionReport takeReport();

 private:
  /**
   * @returns The place of @p sender: the one it had, a free one, or the one of
   * the sender rejected least recently, which is then forgotten. Call it with
   * the lock held.
   */
  [[nodiscard]] RejectedSender& placeOf(const network::Endpoint& sender);

  /** @returns The senders a report names, see RejectionReport::mostRejected.
   * Call it with the lock held. */
  [[nodiscard]] std::vector<RejectedSender> mostRejected() const;

  std::mutex mutex_;
  RejectionCounts sincePreviousReport_{};
  RejectionCounts sinceStart_{};
  std::vector<RejectedSender> senders_;
  std::uint64_t recordedCount_{0};
  std::size_t forgottenSenders_{0};
};

}  // namespace rtype::server
