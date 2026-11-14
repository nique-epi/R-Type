#include "EntitySheetMaskFixtures.hpp"
#include <cstddef>
#include <string>
#include "EntitySheetConstants.hpp"

namespace rtype::game::testing {

std::string maskOfOneRow(std::size_t width) {
  return "[gameplay.mask]\nrows = [\"" + std::string(width, DRAWN_MASK_PIXEL) +
         "\"]\n";
}

}  // namespace rtype::game::testing
