#pragma once

#include <optional>
#include "IncomingDatagram.hpp"
#include "ReceivedStatus.hpp"

namespace rtype::server {

/**
 * @returns The first GameStatus of @p datagram, with the token and the sequence
 * of its header; nothing when the datagram is not of this protocol, holds no
 * status, or holds one its reader refuses. Throws nothing.
 */
[[nodiscard]] std::optional<ReceivedStatus> readGameStatus(
    const network::IncomingDatagram& datagram);

}  // namespace rtype::server
