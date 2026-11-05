#pragma once

#include "Games.hpp"
#include "ServerOccupancy.hpp"

namespace rtype::server {

/**
 * @returns How full @p games are: the players their rosters hold, the number
 * of games, and the empty seats of the games waiting for players, the only
 * seats a ConnectionAcceptor gives. A game that started has no free seat,
 * whatever its roster holds.
 */
[[nodiscard]] ServerOccupancy occupancyOf(const Games& games);

}  // namespace rtype::server
