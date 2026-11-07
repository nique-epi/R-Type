#pragma once

#include "OutgoingDatagram.hpp"
#include "SupervisedGame.hpp"

namespace rtype::server {

/**
 * @returns The order that stops @p game: a Disconnection under its link
 * token, to the port it listens on, on the loopback. The game reads it in its
 * supervisor link module, says goodbye to its players and ends; nothing
 * repeats it, so the caller sends FAREWELL_COPIES of it.
 */
[[nodiscard]] network::OutgoingDatagram stopOrderTo(const SupervisedGame& game);

}  // namespace rtype::server
