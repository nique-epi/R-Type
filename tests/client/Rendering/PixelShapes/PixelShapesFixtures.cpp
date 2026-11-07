#include "PixelShapesFixtures.hpp"
#include <cstddef>
#include <set>
#include <utility>
#include <vector>
#include "ColoredPixel.hpp"

namespace rtype::client::testing {

std::size_t distinctPlacesOf(const std::vector<ColoredPixel>& pixels) {
  std::set<std::pair<int, int>> places;
  for (const ColoredPixel& pixel : pixels) {
    places.emplace(pixel.position.x, pixel.position.y);
  }
  return places.size();
}

}  // namespace rtype::client::testing
