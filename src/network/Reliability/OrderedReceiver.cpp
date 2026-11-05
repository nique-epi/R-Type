#include "OrderedReceiver.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "EncodedMessage.hpp"
#include "MessageType.hpp"
#include "OrderedMessage.hpp"
#include "ProtocolConstants.hpp"
#include "SequenceOrder.hpp"

namespace rtype::network {

std::vector<EncodedMessage> OrderedReceiver::receive(
    const OrderedMessage& message) {
  std::vector<EncodedMessage> ready;
  skipTo(message.firstKept, ready);
  hold(message);
  deliverRun(ready);
  return ready;
}

void OrderedReceiver::reset() {
  slots_.fill(std::nullopt);
  expected_ = FIRST_ORDERED_NUMBER;
}

std::size_t OrderedReceiver::heldCount() const {
  return static_cast<std::size_t>(std::ranges::count_if(
      slots_, [](const std::optional<EncodedMessage>& slot) {
        return slot.has_value();
      }));
}

void OrderedReceiver::skipTo(std::uint16_t firstKept,
                             std::vector<EncodedMessage>& ready) {
  if (!isNewerSequence(firstKept, expected_)) {
    return;
  }
  const std::size_t distance =
      static_cast<std::uint16_t>(firstKept - expected_);
  const std::size_t flushed = std::min(distance, ORDERED_WINDOW);
  for (std::size_t step = 0; step < flushed; ++step) {
    std::optional<EncodedMessage>& slot =
        slotOf(static_cast<std::uint16_t>(expected_ + step));
    if (slot.has_value()) {
      ready.push_back(std::move(*slot));
      slot.reset();
    }
  }
  expected_ = firstKept;
}

void OrderedReceiver::hold(const OrderedMessage& message) {
  const std::size_t ahead =
      static_cast<std::uint16_t>(message.number - expected_);
  std::optional<EncodedMessage>& slot = slotOf(message.number);
  if (ahead >= ORDERED_WINDOW || slot.has_value()) {
    return;
  }
  slot = EncodedMessage{.type = static_cast<MessageType>(message.innerType),
                        .payload = message.payload};
}

void OrderedReceiver::deliverRun(std::vector<EncodedMessage>& ready) {
  for (std::optional<EncodedMessage>* slot = &slotOf(expected_);
       slot->has_value(); slot = &slotOf(expected_)) {
    ready.push_back(std::move(**slot));
    slot->reset();
    ++expected_;
  }
}

std::optional<EncodedMessage>& OrderedReceiver::slotOf(std::uint16_t number) {
  return slots_.at(number % ORDERED_WINDOW);
}

}  // namespace rtype::network
