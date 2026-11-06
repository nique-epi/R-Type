#pragma once

#include <cstddef>
#include "IncomingDatagram.hpp"

namespace rtype::server::testing {

/** @returns A datagram holding a ChatLine from the player at @p index. */
[[nodiscard]] network::IncomingDatagram chatLineFrom(std::size_t index);

/** @returns A datagram from the player at @p index holding a Disconnection,
 * then a ChatLine. */
[[nodiscard]] network::IncomingDatagram disconnectionThenChatLineFrom(
    std::size_t index);

}  // namespace rtype::server::testing
