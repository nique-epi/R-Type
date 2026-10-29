#include "RenderSystemFixtures.hpp"
#include <cstddef>
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
  for (std::size_t frame = 0; frame < flight.trail.size(); ++frame) {
    flight.trail.at(frame) = rtype::game::Position{
        .x = SHIP_X + static_cast<float>(frame), .y = SHIP_Y};
  }
  flight.trailLength = flight.trail.size();
  return flight;
}
