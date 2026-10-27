#include "ShipFlightSystem.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iterator>
#include <optional>
#include "ComponentRegistry.hpp"
#include "Easing.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "ShipControlConstants.hpp"
#include "ShipFlight.hpp"
#include "ShipFlightConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The flame of @p flight after @p elapsed, eased towards the thrust
 * its pilot asks for, as FLAME_RESPONSE_RATE tells. */
float flameAfter(const ShipFlight& flight, engine::Duration elapsed) {
  float wanted = 0.0F;
  if (flight.forward || flight.dashing) {
    wanted = 1.0F;
  } else if (flight.sideways) {
    wanted = FLAME_MANEUVER_SHARE;
  }
  const float seconds = std::chrono::duration<float>(elapsed).count();
  return flight.flame + ((wanted - flight.flame) *
                         (1.0F - std::exp(-FLAME_RESPONSE_RATE * seconds)));
}

/** @returns The share of the roll drawn on @p flight after @p elapsed: 0 when
 * a world state starts saying the ship rolls, nothing once the whole of
 * game::ROLL_DURATION has passed or without a roll. */
std::optional<float> rollShareAfter(const ShipFlight& flight,
                                    engine::Duration elapsed) {
  if (flight.rolling && !flight.rolledBefore) {
    return 0.0F;
  }
  if (!flight.rollShare.has_value()) {
    return std::nullopt;
  }
  const float share = *flight.rollShare + shareOf(elapsed, game::ROLL_DURATION);
  if (share >= 1.0F) {
    return std::nullopt;
  }
  return share;
}

/** @brief Puts @p position at the head of the trail of @p flight, its oldest
 * frame dropped. */
void keepPosition(ShipFlight& flight, game::Position position) {
  std::ranges::rotate(flight.trail, std::prev(flight.trail.end()));
  flight.trail.front() = position;
  flight.trailLength = std::min(flight.trailLength + 1, flight.trail.size());
}

}  // namespace

void ShipFlightSystem::update(engine::ComponentRegistry& components,
                              engine::Duration elapsed) {
  components.forEach<ShipFlight, game::Position>(
      [elapsed](engine::Entity, ShipFlight& flight,
                const game::Position& position) {
        flight.age += elapsed;
        flight.flame = flameAfter(flight, elapsed);
        flight.forwardTime = flight.forward ? flight.forwardTime + elapsed
                                            : engine::Duration::zero();
        flight.rollShare = rollShareAfter(flight, elapsed);
        flight.rolledBefore = flight.rolling;
        keepPosition(flight, position);
      });
}

}  // namespace rtype::client
