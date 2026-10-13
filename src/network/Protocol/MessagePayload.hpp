#pragma once

#include <concepts>
#include <cstddef>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief A message written as one payload: it names its MessageType in TYPE,
 * and an encodePayload(ByteWriter&, const Message&) overload, found next to
 * the message, writes it.
 */
template <typename Message>
concept EncodableMessage =
    requires(ByteWriter& writer, const Message& message) {
      { Message::TYPE } -> std::convertible_to<MessageType>;
      encodePayload(writer, message);
    };

/**
 * @brief A message read back from one payload: it names its MessageType in
 * TYPE, and a decodePayload(payload, std::type_identity<Message>) overload,
 * found next to the message, reads it. The second argument only picks the
 * overload, so a message without a reader fails to compile.
 */
template <typename Message>
concept DecodableMessage = requires(std::span<const std::byte> payload) {
  { Message::TYPE } -> std::convertible_to<MessageType>;
  {
    decodePayload(payload, std::type_identity<Message>{})
  } -> std::same_as<Message>;
};

/**
 * @returns The payload of @p message.
 * @throws What its encodePayload() throws; every encodePayload() checks the
 * whole message before it writes anything.
 */
template <EncodableMessage Message>
std::vector<std::byte> encodeMessage(const Message& message) {
  ByteWriter writer;
  encodePayload(writer, message);
  return writer.bytes();
}

/**
 * @returns The message of type Message that the whole of @p payload holds.
 * @throws A NetworkException when @p payload is not such a message: cut
 * short, holding a value the protocol does not allow, or followed by more
 * bytes.
 */
template <DecodableMessage Message>
Message decodeMessage(std::span<const std::byte> payload) {
  return decodePayload(payload, std::type_identity<Message>{});
}

}  // namespace rtype::network
