#include "TabStripFixtures.hpp"
#include <cstddef>
#include "TabStrip.hpp"
#include "TabStripTestConstants.hpp"

using rtype::client::TabStrip;
using rtype::client::testing::LARGEST_TEXT;
using rtype::client::testing::NARROW_TOP_LEFT;
using rtype::client::testing::NARROW_WIDTH;
using rtype::client::testing::STRIP_TOP_LEFT;

TabStrip optionsTabs(std::size_t open) {
  return {{"SOUND", "CONTROLS", "DISPLAY", "ACCESS"},
          open,
          {.previous = "Q", .next = "E"},
          STRIP_TOP_LEFT};
}

TabStrip fiveOptionsTabsAtLargest(std::size_t open) {
  TabStrip strip({"SOUND", "CONTROLS", "DISPLAY", "ACCESS", "CHAT"}, open,
                 {.previous = "Q", .next = "E"}, STRIP_TOP_LEFT);
  strip.setTextSize(LARGEST_TEXT);
  return strip;
}

TabStrip narrowStrip() {
  TabStrip strip({"SERVER", "GLOBAL"}, 0, {.previous = "Q", .next = "E"},
                 NARROW_TOP_LEFT);
  strip.setWidth(NARROW_WIDTH);
  return strip;
}
