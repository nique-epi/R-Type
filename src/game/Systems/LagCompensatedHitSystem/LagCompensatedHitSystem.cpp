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
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "HurtBox.hpp"
#include "HurtBoxBounds.hpp"
#include "HurtZone.hpp"
#include "HurtsNow.hpp"
#include "Intent.hpp"
#include "LagCompensationConstants.hpp"
#include "MissileHit.hpp"
#include "Overlaps.hpp"
#include "PastEnemy.hpp"
#include "PastTick.hpp"
#include "Position.hpp"
#include "ShipContact.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "ThreatKind.hpp"
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

/** @returns The first box of @p enemies that overlaps @p bounds and whose
 * entity is still an entity of @p components; nothing when none is. */
std::optional<PastEnemy> firstHitAmong(
    const std::vector<PastEnemy>& enemies, const engine::Bounds& bounds,
    const engine::ComponentRegistry& components) {
  const auto hit = std::ranges::find_if(
      enemies, [&bounds, &components](const PastEnemy& enemy) {
        return engine::overlaps(bounds, enemy.bounds) &&
               components.has<EntityType>(enemy.entity);
      });
  if (hit == enemies.end()) {
    return std::nullopt;
  }
  return *hit;
}

/** @returns The first threat of @p past that overlaps @p bounds, a missile or
 * a hurt box before a body, and is still an entity of @p components; nothing
 * when none is. */
std::optional<PastEnemy> firstThreatTo(
    const PastTick& past, const engine::Bounds& bounds,
    const engine::ComponentRegistry& components) {
  const std::optional<PastEnemy> threat =
      firstHitAmong(past.threats, bounds, components);
  return threat.has_value() ? threat
                            : firstHitAmong(past.bodies, bounds, components);
}

/** @brief Adds to @p threats every active box of the HurtZone of each enemy
 * of @p components. */
void recordHurtBoxes(engine::ComponentRegistry& components,
                     std::vector<PastEnemy>& threats) {
  components.forEach<HurtZone, Position>(
      [&components, &threats](engine::Entity enemy, const HurtZone& zone,
                              const Position& position) {
        const auto* const current = components.get<CurrentAction>(enemy);
        for (const HurtBox& box : zone.boxes) {
          if (hurtsNow(box, current)) {
            threats.push_back(PastEnemy{.entity = enemy,
                                        .bounds = hurtBoxBounds(box, position),
                                        .kind = ThreatKind::HurtBox});
          }
        }
      });
}

/** @brief Adds to @p tick the box of every enemy and every missile of an enemy
 * of @p components. */
void recordBodiesAndMissiles(engine::ComponentRegistry& components,
                             PastTick& tick) {
  components.forEach<EntityType, Position>(
      [&components, &tick](engine::Entity entity, const EntityType& side,
                           const Position& position) {
        const auto* const box = components.get<CollisionBox>(entity);
        if (box == nullptr) {
          return;
        }
        const engine::Bounds bounds = boundsOf(position, *box);
        if (side == EntityType::Enemy) {
          tick.bodies.push_back(PastEnemy{
              .entity = entity, .bounds = bounds, .kind = ThreatKind::Body});
        } else if (side == EntityType::EnemyMissile) {
          tick.threats.push_back(PastEnemy{
              .entity = entity, .bounds = bounds, .kind = ThreatKind::Missile});
        }
      });
}

}  // namespace

LagCompensatedHitSystem::LagCompensatedHitSystem(engine::EventBus& events)
    : events_(&events) {}

void LagCompensatedHitSystem::update(engine::ComponentRegistry& components,
                                     engine::Duration elapsed) {
  if (world_ != &components) {
    startWith(components);
  }
  recordEnemies(components);
  testPlayerMissiles(components, elapsed);
  testShips(components);
}

void LagCompensatedHitSystem::testPlayerMissiles(
    engine::ComponentRegistry& components, engine::Duration elapsed) {
  components.forEach<ViewDelay, Position>([this, &components, elapsed](
                                              engine::Entity missile,
                                              const ViewDelay& delay,
                                              const Position& position) {
    const auto* const box = components.get<CollisionBox>(missile);
    const auto* const side = components.get<EntityType>(missile);
    if (box == nullptr || side == nullptr ||
        *side != EntityType::PlayerMissile) {
      return;
    }
    const engine::Bounds swept = sweptBack(
        boundsOf(position, *box), travelOf(components, missile, elapsed));
    if (const std::optional<PastEnemy> target =
            firstHitAmong(enemiesAgo(delay.ticks).bodies, swept, components)) {
      events_->publish(
          MissileHit{.missile = missile, .target = target->entity});
    }
  });
}

void LagCompensatedHitSystem::testShips(engine::ComponentRegistry& components) {
  components.forEach<ViewDelay, Position>([this, &components](
                                              engine::Entity ship,
                                              const ViewDelay& delay,
                                              const Position& position) {
    const auto* const box = components.get<CollisionBox>(ship);
    const auto* const side = components.get<EntityType>(ship);
    if (box == nullptr || side == nullptr || *side != EntityType::Player) {
      return;
    }
    const std::optional<PastEnemy> threat = firstThreatTo(
        enemiesAgo(delay.ticks), boundsOf(position, *box), components);
    if (threat.has_value() && threat->kind == ThreatKind::Missile) {
      events_->publish(MissileHit{.missile = threat->entity, .target = ship});
    } else if (threat.has_value()) {
      events_->publish(ShipContact{.player = ship, .enemy = threat->entity});
    }
  });
}

void LagCompensatedHitSystem::recordEnemies(
    engine::ComponentRegistry& components) {
  newest_ = (newest_ + 1) % KEPT_ENEMY_TICKS;
  recorded_ = std::min(recorded_ + 1, KEPT_ENEMY_TICKS);
  PastTick& tick = history_[newest_];
  tick.bodies.clear();
  tick.threats.clear();
  recordBodiesAndMissiles(components, tick);
  recordHurtBoxes(components, tick.threats);
}

void LagCompensatedHitSystem::startWith(
    const engine::ComponentRegistry& components) {
  for (PastTick& tick : history_) {
    tick.bodies.clear();
    tick.threats.clear();
  }
  recorded_ = 0;
  world_ = &components;
}

const PastTick& LagCompensatedHitSystem::enemiesAgo(
    std::size_t ticksAgo) const {
  const std::size_t back = std::min(ticksAgo, recorded_ - 1);
  return history_[(newest_ + KEPT_ENEMY_TICKS - back) % KEPT_ENEMY_TICKS];
}

}  // namespace rtype::game
