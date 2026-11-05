#include "UnacknowledgedMessages.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "EncodedMessage.hpp"
#include "GuaranteedDelivery.hpp"
#include "MessagePayload.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "OrderedMessage.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SequenceOrder.hpp"
#include "UnacknowledgedMessage.hpp"

namespace rtype::network {

namespace {

bool isDue(const UnacknowledgedMessage& kept) {
  return kept.sentIn.empty() || kept.sinceSent >= GUARANTEED_RESEND_DELAY;
}

}  // namespace

void UnacknowledgedMessages::add(EncodedMessage message) {
  if (message.payload.size() > ORDERED_PAYLOAD_LIMIT) {
    throw DatagramTooLargeException(PACKET_HEADER_SIZE + ORDERED_PREFIX_SIZE +
                                    encodedSize(message));
  }
  const std::uint16_t number = nextNumber_++;
  giveUpBefore(static_cast<std::uint16_t>(number - (ORDERED_WINDOW - 1)));
  messages_.push_back(UnacknowledgedMessage{
      .message = std::move(message), .number = number, .sentIn = {}});
}

void UnacknowledgedMessages::acknowledge(const PacketHeader& header) {
  std::erase_if(messages_, [&header](const UnacknowledgedMessage& kept) {
    return std::ranges::any_of(kept.sentIn, [&header](std::uint16_t sequence) {
      return acknowledges(header, sequence);
    });
  });
}

void UnacknowledgedMessages::advance(std::chrono::nanoseconds elapsed) {
  std::optional<std::uint16_t> newestAged;
  for (UnacknowledgedMessage& kept : messages_) {
    kept.age += elapsed;
    kept.sinceSent += elapsed;
    if (kept.age >= GUARANTEED_MESSAGE_LIFETIME) {
      newestAged = kept.number;
    }
  }
  if (newestAged.has_value()) {
    giveUpBefore(static_cast<std::uint16_t>(*newestAged + 1));
  }
}

bool UnacknowledgedMessages::hasDue() const {
  return std::ranges::any_of(messages_, isDue);
}

std::vector<EncodedMessage> UnacknowledgedMessages::takeDue(
    std::uint16_t sequence) {
  std::vector<EncodedMessage> taken;
  std::size_t size = PACKET_HEADER_SIZE;
  for (UnacknowledgedMessage& kept : messages_) {
    if (!isDue(kept)) {
      continue;
    }
    EncodedMessage envelope = envelopeOf(kept);
    const std::size_t messageSize = encodedSize(envelope);
    if (size + messageSize > MAX_DATAGRAM_SIZE) {
      continue;
    }
    size += messageSize;
    kept.sentIn.push_back(sequence);
    kept.sinceSent = std::chrono::nanoseconds::zero();
    taken.push_back(std::move(envelope));
  }
  return taken;
}

std::size_t UnacknowledgedMessages::count() const { return messages_.size(); }

void UnacknowledgedMessages::giveUpBefore(std::uint16_t first) {
  if (!isNewerSequence(first, firstKept_)) {
    return;
  }
  firstKept_ = first;
  std::erase_if(messages_, [this](const UnacknowledgedMessage& kept) {
    return isNewerSequence(firstKept_, kept.number);
  });
}

EncodedMessage UnacknowledgedMessages::envelopeOf(
    const UnacknowledgedMessage& kept) const {
  return encodedMessage(
      OrderedMessage{.number = kept.number,
                     .firstKept = firstKept_,
                     .innerType = static_cast<std::uint8_t>(kept.message.type),
                     .payload = kept.message.payload});
}

}  // namespace rtype::network
