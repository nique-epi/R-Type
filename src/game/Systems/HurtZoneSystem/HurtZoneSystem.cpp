#include "HurtZoneSystem.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <optional>
#include <vector>
#include "ActionKey.hpp"
#include "BeamLine.hpp"
#include "BeamOverlap.hpp"
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "HurtBeam.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
#include "HurtZone.hpp"
#include "HurtingEnemy.hpp"
#include "Overlaps.hpp"
#include "Position.hpp"
#include "ShipBounds.hpp"
#include "ShipContact.hpp"
#include "Summoning.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns Whether a part of a hurt zone that hurts in @p activeIn, inside
 * @p window, hurts now, the enemy being in @p current: always for a part that
 * names no action. */
bool isActive(const std::optional<ActionKey>& activeIn,
              const HurtWindow& window, const CurrentAction* current) {
  if (!activeIn.has_value()) {
    return true;
  }
  if (current == nullptr || current->key != activeIn) {
    return false;
  }
  return current->since >= window.from &&
         (!window.until.has_value() || current->since < *window.until);
}

/** @returns @p box laid on the enemy standing at @p position. */
engine::Bounds hurtBoxBounds(const HurtBox& box, const Position& position) {
  return engine::Bounds{.centerX = position.x + box.offsetX,
                        .centerY = position.y + box.offsetY,
                        .width = box.width,
                        .height = box.height};
}

/** @returns The angle of @p beam @p since into its phase, in degrees: from
 * fromDegrees at the start of its window to toDegrees at its end, evenly;
 * fromDegrees for a beam whose window has no end. */
float beamDegreesAt(const HurtBeam& beam, engine::Duration since) {
  if (!beam.window.until.has_value()) {
    return beam.fromDegrees;
  }
  const std::chrono::duration<float> sweep =
      *beam.window.until - beam.window.from;
  const std::chrono::duration<float> swept = since - beam.window.from;
  const float share = std::clamp(swept / sweep, 0.0F, 1.0F);
  return beam.fromDegrees + ((beam.toDegrees - beam.fromDegrees) * share);
}

/** @returns @p beam laid on the enemy at @p position, @p since into its
 * phase: from its pivot, heading left turned down by its angle. */
BeamLine beamLineOf(const HurtBeam& beam, const Position& position,
                    engine::Duration since) {
  const float radians = beamDegreesAt(beam, since) * RADIANS_PER_DEGREE;
  return BeamLine{
      .startX = position.x + beam.offsetX,
      .startY = position.y + beam.offsetY,
      .heading = Direction{.x = -std::cos(radians), .y = std::sin(radians)},
      .length = beam.length,
      .thickness = beam.thickness};
}

/** @returns Whether an active box of @p enemy overlaps @p ship. */
bool isHurtByABox(const HurtingEnemy& enemy, const ShipBounds& ship) {
  return std::ranges::any_of(
      enemy.zone.get().boxes, [&](const HurtBox& box) noexcept {
        return isActive(box.activeIn, box.window, enemy.current) &&
               engine::overlaps(hurtBoxBounds(box, enemy.position),
                                ship.bounds);
      });
}

/** @brief Lays in @p lines the active beams of @p enemy, once for all the
 * ships; an active beam names its action, so the enemy is in it. */
void layActiveBeams(const HurtingEnemy& enemy, std::vector<BeamLine>& lines) {
  lines.clear();
  for (const HurtBeam& beam : enemy.zone.get().beams) {
    if (isActive(beam.activeIn, beam.window, enemy.current)) {
      lines.push_back(beamLineOf(beam, enemy.position, enemy.current->since));
    }
  }
}

/** @returns Whether one of the beam @p lines overlaps @p ship. */
bool isHurtByABeam(const std::vector<BeamLine>& lines, const ShipBounds& ship) {
  return std::ranges::any_of(lines, [&](const BeamLine& line) noexcept {
    return beamOverlaps(line, ship.bounds);
  });
}

}  // namespace

HurtZoneSystem::HurtZoneSystem(engine::EventBus& events) : events_(events) {}

void HurtZoneSystem::collectShips(engine::ComponentRegistry& components) {
  ships_.clear();
  components.forEach<EntityType, CollisionBox>(
      [this, &components](engine::Entity entity, const EntityType& type,
                          const CollisionBox& box) {
        const Position* const position = components.get<Position>(entity);
        if (type == EntityType::Player && position != nullptr) {
          ships_.push_back(
              ShipBounds{.entity = entity, .bounds = boundsOf(*position, box)});
        }
      });
}

void HurtZoneSystem::publishContacts(engine::ComponentRegistry& components) {
  components.forEach<HurtZone, Position>(
      [this, &components](engine::Entity enemy, const HurtZone& zone,
                          const Position& position) {
        if (components.has<Summoning>(enemy)) {
          return;
        }
        const HurtingEnemy hurting{
            .zone = zone,
            .position = position,
            .current = components.get<CurrentAction>(enemy)};
        layActiveBeams(hurting, beamLines_);
        for (const ShipBounds& ship : ships_) {
          if (isHurtByABox(hurting, ship) || isHurtByABeam(beamLines_, ship)) {
            events_.publish(ShipContact{.player = ship.entity, .enemy = enemy});
          }
        }
      });
}

void HurtZoneSystem::update(engine::ComponentRegistry& components,
                            [[maybe_unused]] engine::Duration elapsed) {
  collectShips(components);
  if (!ships_.empty()) {
    publishContacts(components);
  }
}

}  // namespace rtype::game
