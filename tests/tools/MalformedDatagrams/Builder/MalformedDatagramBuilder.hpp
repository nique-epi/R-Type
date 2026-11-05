#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>
#include "MalformedDatagramKind.hpp"
#include "PacketHeader.hpp"
#include "SampleMessage.hpp"

namespace rtype::tools {

/**
 * @brief Writes malformed datagrams of each kind, drawn from a generator of
 * its own: two builders made from the same seed write the same datagrams in
 * the same order, so a run can be replayed.
 *
 * Every kind but Oversized and RandomBytes carries the header it is given and
 * starts from a valid message of the protocol, so a receiver that checks the
 * header reads as far as the fault. The tests of the tool check what makes
 * each kind invalid: a Truncated or LyingSize datagram cannot be split into
 * messages, the reader of its type refuses a TooLong or ExtremeValue message,
 * no reader takes an UnknownType message, an Oversized datagram holds more
 * than MAX_DATAGRAM_SIZE bytes and a RandomBytes datagram at most that many.
 */
class MalformedDatagramBuilder {
 public:
  explicit MalformedDatagramBuilder(std::uint32_t seed);

  /**
   * @returns The next datagram of @p kind, carrying @p header when the kind
   * holds one.
   */
  [[nodiscard]] std::vector<std::byte> build(
      MalformedDatagramKind kind, const network::PacketHeader& header);

 private:
  /** @returns A datagram of one sample message, cut before its last byte. */
  [[nodiscard]] std::vector<std::byte> truncated(
      const network::PacketHeader& header);

  /** @returns A sample message followed, inside its own size, by random bytes.
   */
  [[nodiscard]] std::vector<std::byte> tooLong(
      const network::PacketHeader& header);

  /** @returns Random bytes, more than MAX_DATAGRAM_SIZE of them. */
  [[nodiscard]] std::vector<std::byte> oversized();

  /** @returns A message of an unknown type holding random bytes. */
  [[nodiscard]] std::vector<std::byte> unknownType(
      const network::PacketHeader& header);

  /** @returns A sample message whose size announces more bytes than follow.
   */
  [[nodiscard]] std::vector<std::byte> lyingSize(
      const network::PacketHeader& header);

  /** @returns A sample message whose bounded field holds OUT_OF_RANGE_BYTE. */
  [[nodiscard]] std::vector<std::byte> extremeValue(
      const network::PacketHeader& header);

  /** @returns Random bytes, from none to MAX_DATAGRAM_SIZE. */
  [[nodiscard]] std::vector<std::byte> anyBytes();

  std::mt19937 generator_;
  std::vector<SampleMessage> samples_;
};

}  // namespace rtype::tools
