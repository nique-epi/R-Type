#include "RenderSystemFixtures.hpp"
#include <string>
#include <vector>
#include "Position.hpp"
#include "RenderSystemTestConstants.hpp"
#include "ShipFlight.hpp"

std::vector<std::string> allLayersInOrder() {
  return {"background",   "bydo",          "explosion", "ship",
          "figure-plate", "enemy-missile", "score"};
}

rtype::client::ShipFlight halfwayRollingFlight() {
  rtype::client::ShipFlight flight{};
  flight.rolling = true;
  flight.rollShare = HALF_A_ROLL;
  return flight;
}

rtype::client::ShipFlight dashingFlight() {
  rtype::client::ShipFlight flight{};
  flight.dashing = true;
  flight.trail.fill(rtype::game::Position{.x = SHIP_X, .y = SHIP_Y});
  flight.trailLength = flight.trail.size();
  return flight;
}
