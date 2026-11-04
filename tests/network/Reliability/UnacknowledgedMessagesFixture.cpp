#include "UnacknowledgedMessagesFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EncodedMessage.hpp"
#include "PacketHeader.hpp"
#include "ReliabilityFixtures.hpp"
#include "ReliabilityTestConstants.hpp"

namespace rtype::network::testing {

void UnacknowledgedMessagesFixture::runTheDemonstration() {
  for (std::size_t step = 0; step < DEMONSTRATION_STEPS; ++step) {
    if (step < DEMONSTRATION_MESSAGE_COUNT) {
      sender_.add(numberedMessage(static_cast<std::uint16_t>(step)));
    }
    sendDue();
    acknowledge();
    sender_.advance(DEMONSTRATION_STEP);
  }
}

std::size_t UnacknowledgedMessagesFixture::distinctMessagesDelivered() const {
  return delivered_.size();
}

void UnacknowledgedMessagesFixture::sendDue() {
  while (sender_.hasDue()) {
    const std::uint16_t sequence = nextSequence_;
    ++nextSequence_;
    const std::vector<EncodedMessage> due = sender_.takeDue(sequence);
    if (towardReceiver_.losesNext() || !receiver_.accept(sequence)) {
      continue;
    }
    for (const EncodedMessage& message : due) {
      delivered_.insert(numberOf(message));
    }
  }
}

void UnacknowledgedMessagesFixture::acknowledge() {
  PacketHeader header;
  receiver_.acknowledgeIn(header);
  if (!towardSender_.losesNext()) {
    sender_.acknowledge(header);
  }
}

}  // namespace rtype::network::testing
