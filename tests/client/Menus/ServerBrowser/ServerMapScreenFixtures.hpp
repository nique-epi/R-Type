#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "GameListSnapshot.hpp"
#include "GameRow.hpp"
#include "ListedGameState.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerRow.hpp"

namespace rtype::client::testing {

/** @returns Lyon (24 ms), Paris (88 ms) then the old station of another
 * version, as the catalog sorts them. */
[[nodiscard]] std::vector<ServerRow> threeServers();

/** @returns The pixel Paris stands on, the map of threeServers() seen whole. */
[[nodiscard]] sf::Vector2i parisOnTheWholeMap();

/** @returns Whether @p painting holds the minimap: its box filled with INK
 * inside MINIMAP_AREA, which the map paints only while zoomed in. */
[[nodiscard]] bool showsTheMiniMap(const InterfaceCanvasRecord& painting);

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
