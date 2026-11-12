#include "LagCompensatedHitSystem.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <vector>
#include "ActiveHurts.hpp"
#include "ActiveHurtsLaying.hpp"
#include "BeamLine.hpp"
#include "BeamOverlap.hpp"
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollidingBoxes.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "CollisionFilter.hpp"
#include "CollisionFiltering.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityBounds.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "HitboxParts.hpp"
#include "HurtZone.hpp"
#include "HurtingEnemy.hpp"
#include "Intent.hpp"
#include "LagCompensationConstants.hpp"
#include "MissileHit.hpp"
#include "Overlaps.hpp"
#include "PastBeam.hpp"
#include "PastEnemy.hpp"
#include "PastTick.hpp"
#include "Position.hpp"
#include "PreHitCheck.hpp"
#include "ShipContact.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "Summoning.hpp"
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

/** @returns The first box of @p enemies whose filter and the one of
 * @p tester engine::mayCollide() keeps, that overlaps the box of @p tester
 * and whose entity is still an entity of @p components; nothing when none
 * is. */
std::optional<PastEnemy> firstHitAmong(
    const std::vector<PastEnemy>& enemies, const EntityBounds& tester,
    const engine::ComponentRegistry& components) {
  const auto hit = std::ranges::find_if(
      enemies, [&tester, &components](const PastEnemy& enemy) {
        return engine::mayCollide(tester.filter, enemy.filter) &&
               engine::overlaps(tester.bounds, enemy.bounds) &&
               components.has<EntityType>(enemy.entity);
      });
  if (hit == enemies.end()) {
    return std::nullopt;
  }
  return *hit;
}

/** @returns The enemy of the first beam of @p beams whose filter and the one
 * of @p tester engine::mayCollide() keeps, that overlaps the box of @p tester
 * and is still an entity of @p components, as a threat of kind HurtBeam;
 * nothing when none is. */
std::optional<PastEnemy> firstBeamAmong(
    const std::vector<PastBeam>& beams, const EntityBounds& tester,
    const engine::ComponentRegistry& components) {
  const auto hit =
      std::ranges::find_if(beams, [&tester, &components](const PastBeam& beam) {
        return engine::mayCollide(tester.filter, beam.filter) &&
               beamOverlaps(beam.line, tester.bounds) &&
               components.has<EntityType>(beam.entity);
      });
  if (hit == beams.end()) {
    return std::nullopt;
  }
  return PastEnemy{.entity = hit->entity, .kind = ThreatKind::HurtBeam};
}

/** @returns The first threat of @p past that touches @p tester, as
 * firstHitAmong() and firstBeamAmong() tell it: a missile or a hurt box, then
 * a beam, before a body; nothing when none does. */
std::optional<PastEnemy> firstThreatTo(
    const PastTick& past, const EntityBounds& tester,
    const engine::ComponentRegistry& components) {
  if (std::optional<PastEnemy> threat =
          firstHitAmong(past.threats, tester, components)) {
    return threat;
  }
  if (std::optional<PastEnemy> beam =
          firstBeamAmong(past.beams, tester, components)) {
    return beam;
  }
  return firstHitAmong(past.bodies, tester, components);
}

/** @returns @p entity with the box @p bounds and its collision filter;
 * nothing for an entity without a filter, which touches nothing. */
std::optional<EntityBounds> testerOf(engine::ComponentRegistry& components,
                                     engine::Entity entity,
                                     const engine::Bounds& bounds) {
  const auto* const filter = components.get<engine::CollisionFilter>(entity);
  if (filter == nullptr) {
    return std::nullopt;
  }
  return EntityBounds{.entity = entity, .bounds = bounds, .filter = *filter};
}

/** @brief Adds to @p tick every active box and beam of the HurtZone of each
 * enemy of @p components that has a collision filter and is not summoning,
 * laid through @p hurts. */
void recordHurts(engine::ComponentRegistry& components, PastTick& tick,
                 ActiveHurts& hurts) {
  components.forEach<HurtZone, Position>(
      [&components, &tick, &hurts](engine::Entity enemy, const HurtZone& zone,
                                   const Position& position) {
        const auto* const filter =
            components.get<engine::CollisionFilter>(enemy);
        if (filter == nullptr || components.has<Summoning>(enemy)) {
          return;
        }
        layActiveHurts(
            HurtingEnemy{.zone = zone,
                         .position = position,
                         .current = components.get<CurrentAction>(enemy)},
            hurts);
        for (const engine::Bounds& box : hurts.boxes) {
          tick.threats.push_back(PastEnemy{.entity = enemy,
                                           .bounds = box,
                                           .kind = ThreatKind::HurtBox,
                                           .filter = *filter});
        }
        for (const BeamLine& line : hurts.beams) {
          tick.beams.push_back(
              PastBeam{.entity = enemy, .line = line, .filter = *filter});
        }
      });
}

/** @brief Adds to @p tick each box (collidingBoxesOf()) of every enemy that
 * is not summoning and the box of every missile of an enemy of @p components,
 * of those that have a collision filter. */
void recordBodiesAndMissiles(engine::ComponentRegistry& components,
                             PastTick& tick) {
  components.forEach<EntityType, Position>(
      [&components, &tick](engine::Entity entity, const EntityType& side,
                           const Position& position) {
        const bool isABody =
            side == EntityType::Enemy && !components.has<Summoning>(entity);
        if (!isABody && side != EntityType::EnemyMissile) {
          return;
        }
        const auto* const box = components.get<CollisionBox>(entity);
        const auto* const filter =
            components.get<engine::CollisionFilter>(entity);
        if (box == nullptr || filter == nullptr) {
          return;
        }
        for (const CollisionBox& part :
             collidingBoxesOf(*box, components.get<HitboxParts>(entity))) {
          (isABody ? tick.bodies : tick.threats)
              .push_back(PastEnemy{
                  .entity = entity,
                  .bounds = boundsOf(position, part),
                  .kind = isABody ? ThreatKind::Body : ThreatKind::Missile,
                  .filter = *filter});
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
  components.forEach<ViewDelay, Position>(
      [this, &components, elapsed](engine::Entity missile,
                                   const ViewDelay& delay,
                                   const Position& position) {
        const auto* const box = components.get<CollisionBox>(missile);
        const auto* const side = components.get<EntityType>(missile);
        if (box == nullptr || side == nullptr ||
            *side != EntityType::PlayerMissile) {
          return;
        }
        const std::optional<EntityBounds> swept =
            testerOf(components, missile,
                     sweptBack(boundsOf(position, *box),
                               travelOf(components, missile, elapsed)));
        if (!swept.has_value()) {
          return;
        }
        const std::optional<PastEnemy> target =
            firstHitAmong(enemiesAgo(delay.ticks).bodies, *swept, components);
        if (!target.has_value()) {
          return;
        }
        const MissileHit hit{.missile = missile, .target = target->entity};
        if (targetLetsHit(components, hit)) {
          events_->publish(hit);
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
    const std::optional<EntityBounds> tester =
        testerOf(components, ship, boundsOf(position, *box));
    if (!tester.has_value()) {
      return;
    }
    const std::optional<PastEnemy> threat =
        firstThreatTo(enemiesAgo(delay.ticks), *tester, components);
    if (threat.has_value() && threat->kind == ThreatKind::Missile) {
      const MissileHit hit{.missile = threat->entity, .target = ship};
      if (targetLetsHit(components, hit)) {
        events_->publish(hit);
      }
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
  tick.beams.clear();
  recordBodiesAndMissiles(components, tick);
  recordHurts(components, tick, hurts_);
}

void LagCompensatedHitSystem::startWith(
    const engine::ComponentRegistry& components) {
  for (PastTick& tick : history_) {
    tick.bodies.clear();
    tick.threats.clear();
    tick.beams.clear();
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
