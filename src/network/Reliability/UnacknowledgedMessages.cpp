#include "UnacknowledgedMessages.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "EncodedMessage.hpp"
#include "GuaranteedDelivery.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "UnacknowledgedMessage.hpp"

namespace rtype::network {

namespace {

/**
 * @returns The bytes @p message takes in a datagram: its type and the size of
 * its payload (MESSAGE_PREFIX_SIZE), then its payload.
 */
std::size_t bytesInADatagram(const EncodedMessage& message) {
  return MESSAGE_PREFIX_SIZE + message.payload.size();
}

bool isDue(const UnacknowledgedMessage& kept) {
  return kept.sentIn.empty() || kept.sinceSent >= GUARANTEED_RESEND_DELAY;
}

}  // namespace

void UnacknowledgedMessages::add(EncodedMessage message) {
  const std::size_t alone = PACKET_HEADER_SIZE + bytesInADatagram(message);
  if (alone > MAX_DATAGRAM_SIZE) {
    throw DatagramTooLargeException(alone);
  }
  messages_.push_back(
      UnacknowledgedMessage{.message = std::move(message), .sentIn = {}});
}

void UnacknowledgedMessages::acknowledge(const PacketHeader& header) {
  std::erase_if(messages_, [&header](const UnacknowledgedMessage& kept) {
    return std::ranges::any_of(kept.sentIn, [&header](std::uint16_t sequence) {
      return acknowledges(header, sequence);
    });
  });
}

void UnacknowledgedMessages::advance(std::chrono::nanoseconds elapsed) {
  for (UnacknowledgedMessage& kept : messages_) {
    kept.age += elapsed;
    kept.sinceSent += elapsed;
  }
  std::erase_if(messages_, [](const UnacknowledgedMessage& kept) {
    return kept.age >= GUARANTEED_MESSAGE_LIFETIME;
  });
}

bool UnacknowledgedMessages::hasDue() const {
  return std::ranges::any_of(messages_, isDue);
}

std::vector<EncodedMessage> UnacknowledgedMessages::takeDue(
    std::uint16_t sequence) {
  std::vector<EncodedMessage> taken;
  std::size_t size = PACKET_HEADER_SIZE;
  for (UnacknowledgedMessage& kept : messages_) {
    const std::size_t messageSize = bytesInADatagram(kept.message);
    if (!isDue(kept) || size + messageSize > MAX_DATAGRAM_SIZE) {
      continue;
    }
    size += messageSize;
    kept.sentIn.push_back(sequence);
    kept.sinceSent = std::chrono::nanoseconds::zero();
    taken.push_back(kept.message);
  }
  return taken;
}

std::size_t UnacknowledgedMessages::count() const { return messages_.size(); }

}  // namespace rtype::network
