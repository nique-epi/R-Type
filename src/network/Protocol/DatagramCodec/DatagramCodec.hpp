#pragma once

#include <bitset>
#include <cstddef>
#include <optional>
#include <ranges>
#include <span>
#include <vector>
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief Turns a header and a message into the bytes of one datagram, and a
 * received datagram back into its header and its messages.
 *
 * A datagram is the PACKET_HEADER_SIZE bytes of the header, then one or more
 * messages; each message is its type byte, the size of its payload on two
 * bytes, then the payload. Reading the header and splitting the messages are
 * two calls, because a receiver checks the version before it trusts the rest:
 * a datagram of another version gives its header even when its messages
 * cannot be split.
 */
class DatagramCodec {
 public:
  /** @brief The message types a datagram holds, one bit per type value. */
  using MessageTypes = std::bitset<MESSAGE_TYPE_COUNT>;

  /**
   * @brief Encodes one datagram holding @p header and a single message of
   * type @p type with @p payload.
   * @throws DatagramTooLargeException when the datagram would hold more than
   * MAX_DATAGRAM_SIZE bytes, that is a payload over MESSAGE_PAYLOAD_LIMIT.
   */
  [[nodiscard]] static std::vector<std::byte> encode(
      const PacketHeader& header, MessageType type,
      std::span<const std::byte> payload);

  /**
   * @brief Encodes one datagram holding @p header and @p message.
   * @throws What encodeMessage() throws for @p message, before anything is
   * encoded.
   */
  template <EncodableMessage Message>
  [[nodiscard]] static std::vector<std::byte> encode(const PacketHeader& header,
                                                     const Message& message) {
    return encode(header, Message::TYPE, encodeMessage(message));
  }

  /**
   * @brief Reads the header at the start of @p datagram without judging its
   * fields: checking the protocol id, the version and the flags is left to
   * the caller.
   * @throws MalformedDatagramException when @p datagram is shorter than
   * PACKET_HEADER_SIZE.
   */
  [[nodiscard]] static PacketHeader decodeHeader(
      std::span<const std::byte> datagram);

  /**
   * @returns Whether @p header carries the protocol id and the version this
   * code speaks, with no flag set: the first checks of a received datagram,
   * before anything else in it is trusted.
   */
  [[nodiscard]] static bool speaksThisProtocol(const PacketHeader& header);

  /**
   * @returns Whether @p header carries the protocol id of this code with
   * another version: a peer of another version wrote it, whatever the rest of
   * the header means in that version, since protocolId and version keep their
   * offsets in every version.
   */
  [[nodiscard]] static bool speaksAnotherVersion(const PacketHeader& header);

  /**
   * @brief Splits the messages that follow the header of @p datagram, in
   * order.
   *
   * Every size is checked before anything is returned: either the sizes add
   * up exactly to the end of the datagram, or no message is returned. The
   * payloads are views into @p datagram.
   * @throws MalformedDatagramException when @p datagram is shorter than the
   * header, holds no message, has a size running past its end, or has bytes
   * left after its last message.
   */
  [[nodiscard]] static std::vector<MessageFrame> splitMessages(
      std::span<const std::byte> datagram);

  /**
   * @brief Reads the type of each message that follows the header of
   * @p datagram, with the checks of splitMessages(), but throws and allocates
   * nothing: a receiver can sort a datagram from anyone for the cost of
   * reading its message prefixes.
   * @returns The types it holds, or nothing when splitMessages() would throw
   * for it.
   */
  [[nodiscard]] static std::optional<MessageTypes> messageTypes(
      std::span<const std::byte> datagram);

  /**
   * @brief Refused at compile time for a buffer destroyed at the end of the
   * call, such as a temporary std::vector or std::array: the payloads would
   * point into it.
   */
  template <std::ranges::range Buffer>
    requires(!std::ranges::borrowed_range<Buffer>)
  static std::vector<MessageFrame> splitMessages(Buffer&& datagram) = delete;
};

}  // namespace rtype::network
