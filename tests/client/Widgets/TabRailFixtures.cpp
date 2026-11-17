#include "TabRailFixtures.hpp"
#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "TabRail.hpp"
#include "TabRailTab.hpp"
#include "TabRailTestConstants.hpp"

namespace rtype::client::testing {

TabRail threeTabRail() {
  return TabRail(
      std::vector<TabRailTab>{
          TabRailTab{.label = FIRST_RAIL_LABEL, .locked = false},
          TabRailTab{.label = SECOND_RAIL_LABEL, .locked = false},
          TabRailTab{.label = LOCKED_RAIL_LABEL, .locked = true}},
      FIRST_RAIL_TAB, RAIL_TOP_MIDDLE);
}

InterfaceCanvasRecord railPaintingOf(const TabRail& rail) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rail.paint(canvas);
  return record;
}

}  // namespace rtype::client::testing
