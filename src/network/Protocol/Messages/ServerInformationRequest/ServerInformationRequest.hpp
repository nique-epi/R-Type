#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Anyone asks a server what it is and how full it is, outside any
 * session: a client measuring its ping to a server the player looks at, never
 * to every server of the list, or the master that checks a server answers. The
 * server answers with a ServerInformation and keeps nothing.
 *
 * The request is padded with SERVER_INFORMATION_REQUEST_PADDING_SIZE zero
 * bytes, so its datagram is as large as the largest answer: a forged source
 * address never gets more bytes than its forger sent.
 */
struct ServerInformationRequest {
  static constexpr MessageType TYPE = MessageType::ServerInformationRequest;

  /** @brief When it was sent, in microseconds on the clock of its sender,
   * modulo 2^32: the answer echoes it, so the sender reads the round trip on
   * its own clock. */
  std::uint32_t sentAt{};

  bool operator==(const ServerInformationRequest&) const = default;
};

/**
 * @brief Writes sentAt, then SERVER_INFORMATION_REQUEST_PADDING_SIZE bytes of
 * PADDING_BYTE.
 */
void encodePayload(ByteWriter& writer, const ServerInformationRequest& message);

/**
 * @brief Reads a ServerInformationRequest from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short, its padding
 * included: a request too short to pay for its answer is refused.
 * @throws InvalidFieldValueException naming PADDING_FIELD when a byte of the
 * padding is not PADDING_BYTE.
 * @throws TrailingBytesException when bytes follow the padding.
 */
[[nodiscard]] ServerInformationRequest decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ServerInformationRequest> type);

}  // namespace rtype::network
