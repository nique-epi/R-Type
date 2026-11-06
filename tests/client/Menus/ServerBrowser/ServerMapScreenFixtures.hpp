#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "GameListSnapshot.hpp"
#include "GameRow.hpp"
#include "ListedGameState.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerRow.hpp"

namespace rtype::client::testing {

/** @returns Lyon (24 ms), Paris (88 ms) then the old station of another
 * version, as the catalog sorts them. */
[[nodiscard]] std::vector<ServerRow> threeServers();

/** @returns The list of threeServers(), ready. */
[[nodiscard]] ServerListSnapshot readyList();

/** @returns Game @p number of Lyon, named @p name, in @p state. */
[[nodiscard]] GameRow gameOf(std::uint16_t number,
                             network::ListedGameState state);

/** @returns The games of Lyon: the waiting one, then the one in game. */
[[nodiscard]] GameListSnapshot lyonGames();

/** @returns @p count waiting games, numbered from 1. */
[[nodiscard]] GameListSnapshot waitingGames(std::size_t count);

}  // namespace rtype::client::testing
