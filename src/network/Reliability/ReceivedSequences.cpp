#include "ReceivedSequences.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SequenceOrder.hpp"

namespace rtype::network {

namespace {

/**
 * @returns The bits of the datagrams before the newest once the newest moved
 * @p distance ahead, 1 to 32767: the former newest takes bit @p distance - 1,
 * each datagram the bits named moves @p distance bits up, and what moves past
 * ACKNOWLEDGEMENT_BIT_COUNT is no longer named.
 */
std::uint32_t earlierAfterMoving(std::uint32_t earlier,
                                 std::uint16_t distance) {
  if (std::size_t{distance} > ACKNOWLEDGEMENT_BIT_COUNT) {
    return 0;
  }
  const std::uint64_t withFormerNewest = (std::uint64_t{earlier} << 1U) | 1U;
  return static_cast<std::uint32_t>(withFormerNewest << (distance - 1U));
}

}  // namespace

ReceivedSequences ReceivedSequences::startingAt(std::uint16_t sequence) {
  ReceivedSequences sequences;
  sequences.newest_ = sequence;
  return sequences;
}

bool ReceivedSequences::accept(std::uint16_t sequence) {
  if (!newest_.has_value()) {
    newest_ = sequence;
    return true;
  }
  if (!isNewerSequence(sequence, *newest_)) {
    return false;
  }
  earlier_ = earlierAfterMoving(
      earlier_, static_cast<std::uint16_t>(sequence - *newest_));
  newest_ = sequence;
  return true;
}

std::optional<std::uint16_t> ReceivedSequences::newest() const {
  return newest_;
}

void ReceivedSequences::acknowledgeIn(PacketHeader& header) const {
  if (!newest_.has_value()) {
    return;
  }
  header.flags = static_cast<std::uint8_t>(header.flags | ACKNOWLEDGES_FLAG);
  header.acknowledgedSequence = *newest_;
  header.acknowledgementBits = earlier_;
}

}  // namespace rtype::network
