#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <vector>
#include "ColoredPixel.hpp"
#include "IInterfaceCanvas.hpp"
#include "MapView.hpp"
#include "MiniMap.hpp"
#include "MotionPreference.hpp"
#include "PlanetPose.hpp"
#include "ServerTooltip.hpp"
#include "StarMark.hpp"
#include "StarName.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns How far the planets of the servers have turned after
 * @p elapsed, in frames, as StarMark::turn takes it: one more every
 * PLANET_TURN_FRAME_TIME, back to the first after PLANET_TURN_FRAME_COUNT;
 * always the first while @p motion is MotionPreference::Reduced. */
[[nodiscard]] std::size_t planetTurnAfter(engine::Duration elapsed,
                                          MotionPreference motion);

/** @returns The pixels of the planet and its ring in @p pose, inside the map
 * only, for IInterfaceCanvas::plot(). */
[[nodiscard]] std::vector<ColoredPixel> sectorPlanetPixels(PlanetPose pose);

/** @returns The area the bubble @p tooltip covers when painted. */
[[nodiscard]] sf::IntRect tooltipAreaOf(const ServerTooltip& tooltip);

/** @brief Paints the dashed rings of PING_RINGS_MILLISECONDS around the
 * planet in @p view, inside the map, each labelled under the map when there
 * is room. */
void paintPingRings(IInterfaceCanvas& canvas, const MapView& view);

/** @brief Paints the planet of @p star, centered on it, on the frame of its
 * turn: dark and still for another version, ringed in orange when slow, else
 * larger with more pilots. A star near an edge of the map is not painted. */
void paintStar(IInterfaceCanvas& canvas, const StarMark& star);

/** @brief Paints the cyan corners round the chosen star at @p point. */
void paintChosenStar(IInterfaceCanvas& canvas, sf::Vector2i point);

/** @brief Writes @p name in small print, outlined, on its side; nothing for
 * LabelSide::None. */
void paintStarName(IInterfaceCanvas& canvas, const StarName& name);

/** @brief Paints the bubble @p tooltip. */
void paintServerTooltip(IInterfaceCanvas& canvas, const ServerTooltip& tooltip);

/** @returns How wide @p tooltip must be on @p canvas: its own width at the
 * size the fonts are drawn at; above it, at least as wide as its title and
 * its widest line need with their padding. */
[[nodiscard]] int tooltipWidthFor(const IInterfaceCanvas& canvas,
                                  const ServerTooltip& tooltip);

/** @brief Paints @p map, scaled from the map to its frame. */
void paintMiniMap(IInterfaceCanvas& canvas, const MiniMap& map);

/** @brief Paints the four corners of the frame of the map, in edge blue. */
void paintSectorFrame(IInterfaceCanvas& canvas);

}  // namespace rtype::client
