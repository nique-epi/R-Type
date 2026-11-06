#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <ratio>
#include <vector>
#include "MapView.hpp"
#include "StarBearing.hpp"

namespace rtype::client {

/**
 * @returns How far the planet of the map (SECTOR_PLANET_RADIUS) and its ring
 * reach in the direction @p angle, in radians (0 to the right, down positive),
 * at zoom 1: the larger of the planet and the ellipse the ring makes in that
 * direction, plus the fade of the dots and PLANET_REACH_GAP.
 */
[[nodiscard]] double reachOf(double angle);

/** @returns How far past the reach of the planet a server of @p
 * pingMilliseconds stands, at zoom 1; never below 0. */
[[nodiscard]] double spreadDistanceOf(double pingMilliseconds);

/**
 * @returns The pixel of the grid a server of @p bearing stands on in @p view:
 * past the reach of the planet in its direction, then spreadDistanceOf() its
 * ping, its direction spread over the height the map leaves. As the mockup
 * does, the direction is refined twice, since the reach depends on it.
 */
[[nodiscard]] sf::Vector2i placeOf(StarBearing bearing, const MapView& view);

/** @returns The view at @p zoom that puts a server of @p bearing on @p
 * target. */
[[nodiscard]] MapView viewOn(StarBearing bearing, double zoom,
                             sf::Vector2i target);

/**
 * @returns The direction of the server @p serverId at @p ping: the same for
 * the same id and ping, spread over the span the map shows at that ping, at
 * most MAXIMUM_ANGLE_DEGREES each side.
 */
[[nodiscard]] int angleOf(std::int64_t serverId,
                          std::chrono::duration<double, std::milli> ping);

/** @returns The points of the line of equal @p pingMilliseconds around the
 * right half of the planet in @p view, every PING_RING_STEP_DEGREES. */
[[nodiscard]] std::vector<sf::Vector2f> pingContourOf(double pingMilliseconds,
                                                      const MapView& view);

}  // namespace rtype::client
