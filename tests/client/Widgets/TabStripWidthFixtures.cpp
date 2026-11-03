#include "TabStripWidthFixtures.hpp"
#include "TabStrip.hpp"
#include "TabStripWidthTestConstants.hpp"

using rtype::client::TabStrip;
using rtype::client::testing::NARROW_TOP_LEFT;
using rtype::client::testing::NARROW_WIDTH;

TabStrip narrowStrip() {
  TabStrip strip({"SERVER", "GLOBAL"}, 0, {.previous = "Q", .next = "E"},
                 NARROW_TOP_LEFT);
  strip.setWidth(NARROW_WIDTH);
  return strip;
}
