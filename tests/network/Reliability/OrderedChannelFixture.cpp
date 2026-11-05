#include "OrderedChannelFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EncodedMessage.hpp"
#include "PacketHeader.hpp"
#include "ReliabilityFixtures.hpp"
#include "ReliabilityTestConstants.hpp"

namespace rtype::network::testing {

void OrderedChannelFixture::runTheDemonstration() {
  for (std::size_t step = 0; step < DEMONSTRATION_STEPS; ++step) {
    if (step < DEMONSTRATION_MESSAGE_COUNT) {
      sender_.add(numberedMessage(static_cast<std::uint16_t>(step)));
    }
    sendDue();
    acknowledge();
    sender_.advance(DEMONSTRATION_STEP);
  }
}

const std::vector<std::uint16_t>& OrderedChannelFixture::delivered() const {
  return delivered_;
}

void OrderedChannelFixture::sendDue() {
  while (sender_.hasDue()) {
    const std::uint16_t sequence = nextSequence_;
    ++nextSequence_;
    const std::vector<EncodedMessage> due = sender_.takeDue(sequence);
    if (towardReceiver_.losesNext()) {
      continue;
    }
    if (delayedSequence_.has_value()) {
      arrive(*delayedSequence_, delayedMessages_);
      delayedSequence_.reset();
    }
    if (delays_.losesNext()) {
      delayedSequence_ = sequence;
      delayedMessages_ = due;
      continue;
    }
    arrive(sequence, due);
  }
}

void OrderedChannelFixture::arrive(std::uint16_t sequence,
                                   const std::vector<EncodedMessage>& due) {
  if (!sequences_.accept(sequence)) {
    return;
  }
  for (const EncodedMessage& envelope : due) {
    for (const std::uint16_t number :
         carriedNumbers(receiver_.receive(decodedEnvelope(envelope)))) {
      delivered_.push_back(number);
    }
  }
}

void OrderedChannelFixture::acknowledge() {
  PacketHeader header;
  sequences_.acknowledgeIn(header);
  if (!towardSender_.losesNext()) {
    sender_.acknowledge(header);
  }
}

}  // namespace rtype::network::testing
