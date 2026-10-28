#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Moves the look of every ShipFlight on at each frame: eases its flame
 * towards the thrust asked for, counts how long its pilot pushes forward,
 * starts a roll drawn when a world state starts saying the ship rolls and ends
 * it after game::ROLL_DURATION, and keeps where the ship was at its last
 * AFTERIMAGE_TRAIL_LENGTH frames.
 */
class ShipFlightSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::client
