#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief A line of chat the player writes. The client sends it, then again
 * every 500 ms until the PlayerChatLine of its number comes back, one line at a
 * time, and gives up after 5 s (see the protocol page): the client to server
 * direction has no guaranteed channel, so the number tells the server a copy
 * from a new line.
 */
struct ChatLine {
  static constexpr MessageType TYPE = MessageType::ChatLine;

  /** @brief The number of the line among the lines of its player: one more for
   * each new line, wrapping from 65535 to 0; a copy keeps it. */
  std::uint16_t lineNumber{};
  /** @brief 1 to MAXIMUM_CHAT_TEXT_SIZE bytes of printable UTF-8
   * (isValidChatText()). */
  std::string text;

  bool operator==(const ChatLine&) const = default;
};

/**
 * @brief Writes the line number, then the text.
 * @throws InvalidChatTextException when the text breaks isValidChatText();
 * nothing is written then.
 */
void encodePayload(ByteWriter& writer, const ChatLine& message);

/**
 * @brief Reads and checks the fields of a ChatLine, and leaves what follows
 * them in @p reader, for a message that starts with them.
 * @throws What decodePayload() throws, TrailingBytesException aside.
 */
[[nodiscard]] ChatLine readChatLine(ByteReader& reader);

/**
 * @brief Reads a ChatLine from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidChatTextException when the text breaks isValidChatText().
 * @throws TrailingBytesException when bytes follow the text.
 */
[[nodiscard]] ChatLine decodePayload(std::span<const std::byte> payload,
                                     std::type_identity<ChatLine> type);

}  // namespace rtype::network
