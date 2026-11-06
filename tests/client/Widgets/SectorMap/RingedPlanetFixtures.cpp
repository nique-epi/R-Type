#include "RingedPlanetFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cstddef>
#include <vector>
#include "ColoredPixel.hpp"
#include "PlanetPose.hpp"
#include "RingedPlanetTestConstants.hpp"

namespace rtype::client::testing {

PlanetPose mockupPlanet(double spin, double drift) {
  return PlanetPose{.center = {MOCKUP_PICTURE_HALF, MOCKUP_PICTURE_HALF},
                    .radius = MOCKUP_RADIUS,
                    .spin = spin,
                    .drift = drift};
}

std::size_t countOf(const std::vector<ColoredPixel>& pixels, sf::Color color) {
  return static_cast<std::size_t>(
      std::ranges::count(pixels, color, &ColoredPixel::color));
}

}  // namespace rtype::client::testing
