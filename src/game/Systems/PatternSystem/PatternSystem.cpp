#include "PatternSystem.hpp"
#include <chrono>
#include <cmath>
#include <numbers>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "MovementPattern.hpp"
#include "PatternConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

constexpr float FULL_TURN_RADIANS = 2.0F * std::numbers::pi_v<float>;

float heightOffset(PatternShape shape, float ageSeconds) {
  switch (shape) {
    case PatternShape::Straight:
      return 0.0F;
    case PatternShape::Wave:
      return WAVE_AMPLITUDE *
             std::sin(FULL_TURN_RADIANS * ageSeconds / WAVE_PERIOD_SECONDS);
  }
  return 0.0F;
}

}  // namespace

void PatternSystem::update(engine::ComponentRegistry& components,
                           engine::Duration elapsed) {
  const float elapsedSeconds = std::chrono::duration<float>(elapsed).count();
  components.forEach<Position, MovementPattern>(
      [elapsedSeconds](engine::Entity, Position& position,
                       MovementPattern& pattern) {
        pattern.ageSeconds += elapsedSeconds;
        position.y =
            pattern.originY + heightOffset(pattern.shape, pattern.ageSeconds);
      });
}

}  // namespace rtype::game
