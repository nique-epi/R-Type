#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string_view>
#include "ManeuverKind.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
#include "ShipMotion.hpp"

namespace rtype::game::testing {

/** @brief Two opposite directions held together, named after their axis. */
struct OppositeDirections {
  std::string_view name;
  ShipDirections held;
};

/** @brief A ship heading for an edge, named after it, and where it must stop:
 * with its side on that edge. */
struct EdgeApproach {
  std::string_view name;
  ShipDirections held;
  Position from{};
  Position stop{};
};

/** @brief A direction pressed twice, named after it, and the maneuver it
 * starts. */
struct TappedDirection {
  std::string_view name;
  ShipDirections held;
  ManeuverKind kind{ManeuverKind::None};
  Position distance{};
};

/** @brief Prints a case by its name, so the name of each test says which case
 * it runs. */
void PrintTo(const OppositeDirections& directions, std::ostream* output);
void PrintTo(const EdgeApproach& approach, std::ostream* output);
void PrintTo(const TappedDirection& tapped, std::ostream* output);

class ShipControlOppositesTest
    : public ::testing::TestWithParam<OppositeDirections> {};

class ShipControlEdgeTest : public ::testing::TestWithParam<EdgeApproach> {};

class ShipControlTapTest : public ::testing::TestWithParam<TappedDirection> {};

/** @returns @p ship after @p ticks of SIMULATION_TICK_DURATION, its player
 * holding @p held. */
MovingShip shipAfterTicks(MovingShip ship, ShipDirections held,
                          std::size_t ticks);

/** @returns @p from once @p held is pressed at FIRST_PRESS, let go at
 * RELEASE, and pressed again at @p secondPress. */
ShipMotion motionAfterTwoPresses(ShipMotion from, ShipDirections held,
                                 std::uint32_t secondPress);

}  // namespace rtype::game::testing
