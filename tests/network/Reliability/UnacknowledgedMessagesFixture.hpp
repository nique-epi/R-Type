#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <set>
#include "ReceivedSequences.hpp"
#include "ReliabilityTestConstants.hpp"
#include "SeededLoss.hpp"
#include "UnacknowledgedMessages.hpp"

namespace rtype::network::testing {

/**
 * @brief A sender of guaranteed messages and its receiver, on a link that
 * loses ONE_LOSS_IN_FIVE of the datagrams each way, drawn from fixed seeds. At
 * each step the sender gives the next message while some are left and sends
 * what is due, the receiver reads what got through and sends one
 * acknowledgement back, then DEMONSTRATION_STEP passes. A datagram that would
 * arrive late or twice is refused by ReceivedSequences as a lost one is, so the
 * losses stand for the reordering and the duplicates of UDP too.
 */
class UnacknowledgedMessagesFixture : public ::testing::Test {
 protected:
  /** @brief Runs DEMONSTRATION_STEPS steps; each of the first
   * DEMONSTRATION_MESSAGE_COUNT gives one more message. */
  void runTheDemonstration();

  /** @returns How many different messages the receiver got. */
  [[nodiscard]] std::size_t distinctMessagesDelivered() const;

 private:
  void sendDue();
  void acknowledge();

  UnacknowledgedMessages sender_;
  ReceivedSequences receiver_;
  SeededLoss towardReceiver_{TOWARD_RECEIVER_SEED, ONE_LOSS_IN_FIVE};
  SeededLoss towardSender_{TOWARD_SENDER_SEED, ONE_LOSS_IN_FIVE};
  std::set<std::uint16_t> delivered_;
  std::uint16_t nextSequence_{0};
};

}  // namespace rtype::network::testing
