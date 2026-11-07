#include "LagCompensatedHitSystem.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <vector>
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "Intent.hpp"
#include "LagCompensationConstants.hpp"
#include "MissileHit.hpp"
#include "Overlaps.hpp"
#include "PastEnemy.hpp"
#include "Position.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "TimeConstants.hpp"
#include "ViewDelay.hpp"

namespace rtype::game {

namespace {

/** @returns How far @p body moved along each axis during @p elapsed at its
 * steady speed; nothing for a body without SteadyMotion or Intent. */
Position travelOf(engine::ComponentRegistry& components, engine::Entity body,
                  engine::Duration elapsed) {
  const auto* const motion = components.get<SteadyMotion>(body);
  const auto* const intent = components.get<Intent>(body);
  if (motion == nullptr || intent == nullptr) {
    return Position{};
  }
  return steadyTravel(*motion, intent->direction, elapsed);
}

/** @returns @p now stretched back by @p travel, so it covers every place the
 * box went through during the tick. */
engine::Bounds sweptBack(engine::Bounds now, Position travel) {
  return engine::Bounds{.centerX = now.centerX - (travel.x * engine::HALF),
                        .centerY = now.centerY - (travel.y * engine::HALF),
                        .width = now.width + std::abs(travel.x),
                        .height = now.height + std::abs(travel.y)};
}

/** @returns The first enemy of @p enemies that @p missile overlaps and that is
 * still an enemy of @p components; nothing when none is. */
std::optional<engine::Entity> firstHitAmong(
    const std::vector<PastEnemy>& enemies, const engine::Bounds& missile,
    const engine::ComponentRegistry& components) {
  const auto hit = std::ranges::find_if(
      enemies, [&missile, &components](const PastEnemy& enemy) {
        return engine::overlaps(missile, enemy.bounds) &&
               components.has<EntityType>(enemy.entity);
      });
  if (hit == enemies.end()) {
    return std::nullopt;
  }
  return hit->entity;
}

}  // namespace

LagCompensatedHitSystem::LagCompensatedHitSystem(engine::EventBus& events)
    : events_(&events) {}

void LagCompensatedHitSystem::update(engine::ComponentRegistry& components,
                                     engine::Duration elapsed) {
  recordEnemies(components);
  components.forEach<ViewDelay, Position>(
      [this, &components, elapsed](engine::Entity missile,
                                   const ViewDelay& delay,
                                   const Position& position) {
        const auto* const box = components.get<CollisionBox>(missile);
        if (box == nullptr) {
          return;
        }
        const engine::Bounds swept = sweptBack(
            boundsOf(position, *box), travelOf(components, missile, elapsed));
        if (const std::optional<engine::Entity> target =
                firstHitAmong(enemiesAgo(delay.ticks), swept, components)) {
          events_->publish(MissileHit{.missile = missile, .target = *target});
        }
      });
}

void LagCompensatedHitSystem::recordEnemies(
    engine::ComponentRegistry& components) {
  newest_ = (newest_ + 1) % KEPT_ENEMY_TICKS;
  recorded_ = std::min(recorded_ + 1, KEPT_ENEMY_TICKS);
  std::vector<PastEnemy>& enemies = history_[newest_];
  enemies.clear();
  components.forEach<EntityType, Position>(
      [&components, &enemies](engine::Entity entity, const EntityType& side,
                              const Position& position) {
        const auto* const box = components.get<CollisionBox>(entity);
        if (side == EntityType::Enemy && box != nullptr) {
          enemies.push_back(
              PastEnemy{.entity = entity, .bounds = boundsOf(position, *box)});
        }
      });
}

const std::vector<PastEnemy>& LagCompensatedHitSystem::enemiesAgo(
    std::size_t ticksAgo) const {
  const std::size_t back = std::min(ticksAgo, recorded_ - 1);
  return history_[(newest_ + KEPT_ENEMY_TICKS - back) % KEPT_ENEMY_TICKS];
}

}  // namespace rtype::game
