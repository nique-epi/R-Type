#include "ShipControlFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <ostream>
#include "ShipControl.hpp"
#include "ShipControlTestConstants.hpp"
#include "ShipMotion.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

void PrintTo(const OppositeDirections& directions, std::ostream* output) {
  *output << directions.name;
}

void PrintTo(const EdgeApproach& approach, std::ostream* output) {
  *output << approach.name;
}

void PrintTo(const TappedDirection& tapped, std::ostream* output) {
  *output << tapped.name;
}

MovingShip shipAfterTicks(MovingShip ship, ShipDirections held,
                          std::size_t ticks) {
  for (std::size_t tick = 0; tick < ticks; ++tick) {
    ship = shipAfter(ship, held, engine::SIMULATION_TICK_DURATION);
  }
  return ship;
}

ShipMotion motionAfterTwoPresses(ShipMotion from, ShipDirections held,
                                 std::uint32_t secondPress) {
  ShipMotion motion = motionAfterInput(
      from, HeldChange{.before = {}, .now = held, .inputNumber = FIRST_PRESS});
  motion = motionAfterInput(
      motion, HeldChange{.before = held, .now = {}, .inputNumber = RELEASE});
  return motionAfterInput(
      motion,
      HeldChange{.before = {}, .now = held, .inputNumber = secondPress});
}

}  // namespace rtype::game::testing
