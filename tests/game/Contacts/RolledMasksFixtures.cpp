#include "RolledMasksFixtures.hpp"
#include "MaskPixel.hpp"
#include "PixelMask.hpp"

namespace rtype::game::testing {

engine::PixelMask diagonalMask(int side) {
  engine::PixelMask mask{side, side};
  for (int row = 0; row < side; ++row) {
    mask.markDrawn(engine::MaskPixel{.x = row, .y = row});
  }
  return mask;
}

}  // namespace rtype::game::testing
