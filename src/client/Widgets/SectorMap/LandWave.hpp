#pragma once

namespace rtype::client {

/**
 * @brief One wave of the lands of the planet: weight * sin(longitude * lon +
 * latitude * l + phase), where l is the latitude, or sin(latitudeBend * lat)
 * when latitudeBend is not 0. Waves of whole longitudes wrap round the planet
 * with no seam.
 */
struct LandWave {
  double longitude{};
  double latitude{};
  double phase{};
  double weight{};
  double latitudeBend{};
};

}  // namespace rtype::client
