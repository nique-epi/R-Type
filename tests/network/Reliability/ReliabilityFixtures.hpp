#pragma once

#include <cstdint>
#include <vector>
#include "EncodedMessage.hpp"
#include "OrderedMessage.hpp"
#include "PacketHeader.hpp"

namespace rtype::network::testing {

/** @returns A message whose payload is @p number on two bytes, so a test
 * tells the messages apart. */
[[nodiscard]] EncodedMessage numberedMessage(std::uint16_t number);

/** @returns The number numberedMessage() wrote in @p message. */
[[nodiscard]] std::uint16_t numberOf(const EncodedMessage& message);

/** @returns The envelope that carries numberedMessage(@p position) as message
 * @p position of the ordered channel, its sender keeping @p oldestKept. */
[[nodiscard]] OrderedMessage envelopeOf(std::uint16_t position,
                                        std::uint16_t oldestKept);

/** @returns The numbers numberedMessage() wrote in @p messages, in order. */
[[nodiscard]] std::vector<std::uint16_t> carriedNumbers(
    const std::vector<EncodedMessage>& messages);

/** @returns The OrderedMessage that the envelope @p message holds. */
[[nodiscard]] OrderedMessage decodedEnvelope(const EncodedMessage& message);

/** @returns A header that acknowledges @p acknowledgedSequence, and the
 * datagrams before it whose bits @p bits sets. */
[[nodiscard]] PacketHeader acknowledgingHeader(
    std::uint16_t acknowledgedSequence, std::uint32_t bits);

}  // namespace rtype::network::testing
