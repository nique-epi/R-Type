#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <vector>
#include "ColoredPixel.hpp"
#include "PlanetPose.hpp"

namespace rtype::client::testing {

/** @returns The planet of radius MOCKUP_RADIUS whose picture starts at the
 * corner of the grid, turned by @p spin, its grains by @p drift. */
[[nodiscard]] PlanetPose mockupPlanet(double spin, double drift);

/** @returns How many of @p pixels are of @p color. */
[[nodiscard]] std::size_t countOf(const std::vector<ColoredPixel>& pixels,
                                  sf::Color color);

}  // namespace rtype::client::testing
