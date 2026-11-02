#pragma once

#include "EncodedMessage.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"

namespace rtype::server::testing {

/** @returns The PlayerLeft of the second player, encoded. */
[[nodiscard]] network::EncodedMessage playerLeftMessage();

/** @returns The EntityDestroyed of a killed monster, encoded. */
[[nodiscard]] network::EncodedMessage entityDestroyedMessage();

/** @returns The header of @p datagram. */
[[nodiscard]] network::PacketHeader headerOf(
    const network::OutgoingDatagram& datagram);

/** @returns How many messages @p datagram holds. */
[[nodiscard]] std::size_t messageCountIn(
    const network::OutgoingDatagram& datagram);

}  // namespace rtype::server::testing
