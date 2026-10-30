#pragma once

#include <span>
#include <vector>
#include "IncomingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Replaces the content of @p kept with the datagrams of @p datagrams
 * whose sender has a seat in @p roster, oldest first, so a game reads the
 * datagrams of its own players only. @p kept keeps its capacity from one call
 * to the next.
 */
void keepDatagramsOf(PlayerRoster& roster,
                     std::span<const network::IncomingDatagram> datagrams,
                     std::vector<network::IncomingDatagram>& kept);

}  // namespace rtype::server
