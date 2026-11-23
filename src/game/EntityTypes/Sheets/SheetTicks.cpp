#include "SheetTicks.hpp"
#include <cstddef>
#include "EntitySheetConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

std::size_t nearestTicksIn(engine::Duration duration) {
  return static_cast<std::size_t>((duration + HALF_A_TICK) /
                                  engine::SIMULATION_TICK_DURATION);
}

}  // namespace rtype::game
