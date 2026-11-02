#pragma once

#include <cstddef>
#include <cstdint>
#include "EncodedMessage.hpp"
#include "PacketHeader.hpp"

namespace rtype::network::testing {

/** @returns A message whose payload is @p number on two bytes, so a test
 * tells the messages apart. */
[[nodiscard]] EncodedMessage numberedMessage(std::uint16_t number);

/** @returns The number numberedMessage() wrote in @p message. */
[[nodiscard]] std::uint16_t numberOf(const EncodedMessage& message);

/** @returns A message whose payload holds @p size zero bytes. */
[[nodiscard]] EncodedMessage messageOfSize(std::size_t size);

/** @returns A header that acknowledges @p acknowledgedSequence, and the
 * datagrams before it whose bits @p bits sets. */
[[nodiscard]] PacketHeader acknowledgingHeader(
    std::uint16_t acknowledgedSequence, std::uint32_t bits);

}  // namespace rtype::network::testing
