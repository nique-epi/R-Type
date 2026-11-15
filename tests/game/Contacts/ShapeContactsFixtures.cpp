#include "ShapeContactsFixtures.hpp"
#include "PixelMask.hpp"
#include "ShapeContactsTestConstants.hpp"

namespace rtype::game::testing {

engine::PixelMask leftPixelDrawn() {
  engine::PixelMask mask{TWO_PIXELS_WIDE, ONE_PIXEL_TALL};
  mask.markDrawn(LEFT_PIXEL);
  return mask;
}

}  // namespace rtype::game::testing
