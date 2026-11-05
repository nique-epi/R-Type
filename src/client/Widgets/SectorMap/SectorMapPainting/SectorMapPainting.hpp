#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "IInterfaceCanvas.hpp"
#include "MapView.hpp"
#include "MiniMap.hpp"
#include "PlanetPose.hpp"
#include "ServerTooltip.hpp"
#include "StarMark.hpp"
#include "StarName.hpp"

namespace rtype::client {

/** @brief Paints the planet and its ring in @p pose, inside the map only, in
 * one draw. */
void paintSectorPlanet(IInterfaceCanvas& canvas, PlanetPose pose);

/** @brief Paints the dashed rings of PING_RINGS_MILLISECONDS around the
 * planet in @p view, inside the map, each labelled under the map when there
 * is room. */
void paintPingRings(IInterfaceCanvas& canvas, const MapView& view);

/** @brief Paints the glyph of @p star, as the mockup draws it: hollow and blue
 * for another version, orange when slow, blue when empty, white otherwise,
 * larger with more pilots. A star near an edge of the map is not painted. */
void paintStar(IInterfaceCanvas& canvas, const StarMark& star);

/** @brief Paints the cyan corners round the chosen star at @p point. */
void paintChosenStar(IInterfaceCanvas& canvas, sf::Vector2i point);

/** @brief Writes @p name in small print, outlined, on its side; nothing for
 * LabelSide::None. */
void paintStarName(IInterfaceCanvas& canvas, const StarName& name);

/** @brief Paints the bubble @p tooltip. */
void paintServerTooltip(IInterfaceCanvas& canvas, const ServerTooltip& tooltip);

/** @brief Paints the loupe at @p topLeft, @p zoom written under it. */
void paintZoomLoupe(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                    std::string_view zoom);

/** @brief Paints @p map, scaled from the map to its frame. */
void paintMiniMap(IInterfaceCanvas& canvas, const MiniMap& map);

/** @brief Paints the four corners of the frame of the map, in edge blue. */
void paintSectorFrame(IInterfaceCanvas& canvas);

}  // namespace rtype::client
