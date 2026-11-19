#pragma once

#include <cstddef>
#include <vector>
#include "LobbyMap.hpp"
#include "LobbyState.hpp"

namespace rtype::server::testing {

/** @returns The line of the room for the player at @p index, under the test
 * nickname. */
[[nodiscard]] network::LobbyPlayer lobbyPlayer(std::size_t index, bool ready);

/** @returns The maps the fixture announces: FIRST_ANNOUNCED_MAP and
 * SECOND_ANNOUNCED_MAP, with their previews. */
[[nodiscard]] std::vector<network::LobbyMap> announcedMaps();

/** @returns The room the fixture announces with @p players: the
 * announcedMaps() after them. */
[[nodiscard]] network::LobbyState announcedRoom(
    std::vector<network::LobbyPlayer> players);

/** @returns How many ticks fit in the longest gap the protocol allows. */
[[nodiscard]] std::size_t ticksInTheLongestGap();

}  // namespace rtype::server::testing
