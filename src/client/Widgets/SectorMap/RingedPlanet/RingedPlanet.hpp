#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "ColoredPixel.hpp"
#include "MotionPreference.hpp"
#include "PlanetPose.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @returns The pixels of the planet of the map and its ring in @p pose, inside
 * @p clip only, as the mockup draws them: a halftone of dots lit from the top
 * right, its lands turning about an axis that leans in "/", inside a ring that
 * reads "\", partly clear in front of the planet, hidden behind it, and shaded
 * where the planet stands between it and the sun.
 */
[[nodiscard]] std::vector<ColoredPixel> ringedPlanetPixels(PlanetPose pose,
                                                           sf::IntRect clip);

/**
 * @returns How far the planet and the grains of its ring have turned after
 * @p elapsed, in radians, at PLANET_FRAMES_PER_SECOND: one turn in
 * PLANET_TURN and one in RING_TURN. Under MotionPreference::Reduced they never
 * turn.
 * @param pose The planet to turn; only its spin and drift change.
 */
[[nodiscard]] PlanetPose turnedPlanet(PlanetPose pose, engine::Duration elapsed,
                                      MotionPreference motion);

}  // namespace rtype::client
