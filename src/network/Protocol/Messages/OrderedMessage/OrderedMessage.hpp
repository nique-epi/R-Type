#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The envelope of a message sent on the ordered channel of a session:
 * the number of the carried message, the oldest number its sender still keeps
 * (firstKept: a receiver waiting for an older one stops waiting), the type
 * byte of the carried message and its payload.
 *
 * The carried message is any message but another OrderedMessage. A receiver
 * hands the carried messages to their handlers in the order of their numbers,
 * see OrderedReceiver.
 */
struct OrderedMessage {
  static constexpr MessageType TYPE = MessageType::OrderedMessage;

  std::uint16_t number;
  std::uint16_t firstKept;
  std::uint8_t innerType;
  std::vector<std::byte> payload;

  bool operator==(const OrderedMessage&) const = default;
};

/**
 * @brief Writes the number, firstKept, the type of the carried message, then
 * its payload as it is.
 * @throws InvalidFieldValueException when the carried type is
 * OrderedMessage, when firstKept is later than the number, or when the payload
 * is over ORDERED_PAYLOAD_LIMIT; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const OrderedMessage& message);

/**
 * @brief Reads an OrderedMessage from the whole of @p payload; the carried
 * payload is what follows the type byte.
 * @throws BufferUnderflowException when @p payload is shorter than
 * ORDERED_PREFIX_SIZE.
 * @throws InvalidFieldValueException for the faults encodePayload() refuses.
 */
[[nodiscard]] OrderedMessage decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<OrderedMessage> type);

}  // namespace rtype::network
