#pragma once

#include <cstdint>
#include "PacketHeader.hpp"

namespace rtype::network {

/**
 * @returns Whether @p header acknowledges the datagram of sequence @p sequence
 * sent to its sender: it carries ACKNOWLEDGES_FLAG, and @p sequence is
 * acknowledgedSequence, or 1 to ACKNOWLEDGEMENT_BIT_COUNT before it with its
 * bit of acknowledgementBits set. A sequence after acknowledgedSequence, or
 * more than ACKNOWLEDGEMENT_BIT_COUNT before it, is not acknowledged. Only read
 * a header of a datagram that passed every check of its receiver, so a forged
 * one acknowledges nothing.
 */
[[nodiscard]] bool acknowledges(const PacketHeader& header,
                                std::uint16_t sequence);

/**
 * @returns Whether @p header carries GUARANTEED_FLAG: its datagram holds
 * guaranteed messages, which its receiver acknowledges within
 * ACKNOWLEDGEMENT_DELAY.
 */
[[nodiscard]] bool holdsGuaranteedMessages(const PacketHeader& header);

/**
 * @brief Sets GUARANTEED_FLAG in @p header, next to the flags it has: its
 * datagram holds guaranteed messages.
 */
void markGuaranteed(PacketHeader& header);

}  // namespace rtype::network
