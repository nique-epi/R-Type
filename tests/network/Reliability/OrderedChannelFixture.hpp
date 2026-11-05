#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "EncodedMessage.hpp"
#include "OrderedReceiver.hpp"
#include "ReceivedSequences.hpp"
#include "ReliabilityTestConstants.hpp"
#include "SeededLoss.hpp"
#include "UnacknowledgedMessages.hpp"

namespace rtype::network::testing {

/**
 * @brief A sender of ordered messages and its receiver, on a link that loses
 * ONE_LOSS_IN_FIVE of the datagrams each way and delays THREE_DELAYS_IN_TEN of
 * those toward the receiver until the next one has arrived, from fixed seeds.
 * A delayed datagram is refused by ReceivedSequences as a late one, so the
 * link loses, delays and reorders. At each step the sender gives the next
 * message while some are left and sends what is due, the receiver reads what
 * got through and sends one acknowledgement back, then DEMONSTRATION_STEP
 * passes. Every message is numbered by its step.
 */
class OrderedChannelFixture : public ::testing::Test {
 protected:
  /** @brief Runs DEMONSTRATION_STEPS steps; each of the first
   * DEMONSTRATION_MESSAGE_COUNT gives one more message. */
  void runTheDemonstration();

  /** @returns The numbers of the messages the receiver handed over, in the
   * order it handed them over. */
  [[nodiscard]] const std::vector<std::uint16_t>& delivered() const;

 private:
  void sendDue();
  void arrive(std::uint16_t sequence, const std::vector<EncodedMessage>& due);
  void acknowledge();

  UnacknowledgedMessages sender_;
  ReceivedSequences sequences_;
  OrderedReceiver receiver_;
  SeededLoss towardReceiver_{TOWARD_RECEIVER_SEED, ONE_LOSS_IN_FIVE};
  SeededLoss towardSender_{TOWARD_SENDER_SEED, ONE_LOSS_IN_FIVE};
  SeededLoss delays_{DELAY_SEED, THREE_DELAYS_IN_TEN};
  std::vector<std::uint16_t> delivered_;
  std::optional<std::uint16_t> delayedSequence_;
  std::vector<EncodedMessage> delayedMessages_;
  std::uint16_t nextSequence_{0};
};

}  // namespace rtype::network::testing
