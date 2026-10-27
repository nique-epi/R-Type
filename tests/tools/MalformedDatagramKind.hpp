#pragma once

#include <cstdint>

namespace rtype::tools {

/**
 * @brief The kinds of invalid datagram the tool sends, one for each case a
 * receiver must survive. Every kind but Oversized and RandomBytes starts from
 * a valid message of the protocol, so a receiver reads as far as the fault.
 */
enum class MalformedDatagramKind : std::uint8_t {
  /** @brief A datagram cut short, anywhere before its last byte. */
  Truncated,
  /** @brief A message whose payload holds more bytes than its type allows. */
  TooLong,
  /** @brief More bytes than MAX_DATAGRAM_SIZE. */
  Oversized,
  /** @brief A message whose type byte the protocol does not list. */
  UnknownType,
  /** @brief A message whose size announces more bytes than follow it. */
  LyingSize,
  /** @brief A message with a field the protocol bounds set to 255. */
  ExtremeValue,
  /** @brief Any bytes, up to MAX_DATAGRAM_SIZE. */
  RandomBytes,
};

}  // namespace rtype::tools
