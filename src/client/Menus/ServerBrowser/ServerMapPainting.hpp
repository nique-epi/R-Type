#pragma once

#include <cstddef>
#include <string_view>
#include "IInterfaceCanvas.hpp"
#include "MapView.hpp"
#include "PlanetPose.hpp"
#include "ServerListPhase.hpp"
#include "ServerMapChoice.hpp"
#include "ServerMapLook.hpp"

namespace rtype::client {

/** @brief Writes the title of the map and the line under it, which counts
 * @p serverCount servers. */
void paintServerMapHeader(IInterfaceCanvas& canvas, std::size_t serverCount);

/** @brief Writes @p master, the master the map reads as a player reads it,
 * on the right of the title, ending at the right edge of the chips. */
void paintMasterAddress(IInterfaceCanvas& canvas, std::string_view master);

/** @brief Writes, in the middle of the map, what it shows instead of servers
 * in @p phase: searching, none online, or the master unreachable; nothing
 * when the list is ready. */
void paintServerMapMessage(IInterfaceCanvas& canvas, ServerListPhase phase);

/** @returns @p turned, standing where the planet stands in @p view, as large
 * as the zoom of @p view makes it. */
[[nodiscard]] PlanetPose planetPoseIn(const MapView& view, PlanetPose turned);

/**
 * @brief Paints the map as @p choice sees it, as the mockup draws it, over the
 * planet the caller plotted: the rings of ping, the planet of every server on
 * the frame of @p look.turn, the names that find a free side and the minimap
 * while zoomed in.
 * The corners and the bubble of the chosen server are painted while zoomed
 * in, and on the whole map only when @p look.chosenShown: the pointer is over
 * it, or a key chose it.
 */
void paintServerMap(IInterfaceCanvas& canvas, const ServerMapChoice& choice,
                    ServerMapLook look);

}  // namespace rtype::client
