#include "BayerThreshold.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include "DitherConstants.hpp"

namespace rtype::client {

namespace {

/** @returns @p value modulo BAYER_SIZE, 0 to BAYER_SIZE - 1 even for a
 * negative value. */
std::size_t bayerIndexOf(int value) {
  return static_cast<std::size_t>(((value % BAYER_SIZE) + BAYER_SIZE) %
                                  BAYER_SIZE);
}

}  // namespace

double bayerThresholdAt(sf::Vector2i pixel) {
  const int level =
      BAYER_MATRIX.at(bayerIndexOf(pixel.y)).at(bayerIndexOf(pixel.x));
  return (static_cast<double>(level) + BAYER_CELL_CENTRE) / BAYER_LEVELS;
}

}  // namespace rtype::client
