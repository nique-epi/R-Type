#include "ActiveHurtsLaying.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <optional>
#include "ActionKey.hpp"
#include "ActiveHurts.hpp"
#include "BeamLine.hpp"
#include "BeamOverlap.hpp"
#include "Bounds.hpp"
#include "CurrentAction.hpp"
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "HurtBeam.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
#include "HurtingEnemy.hpp"
#include "Overlaps.hpp"
#include "Position.hpp"
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

}  // namespace

void layActiveHurts(const HurtingEnemy& enemy, ActiveHurts& hurts) {
  hurts.boxes.clear();
  hurts.beams.clear();
  for (const HurtBox& box : enemy.zone.get().boxes) {
    if (isActive(box.activeIn, box.window, enemy.current)) {
      hurts.boxes.push_back(hurtBoxBounds(box, enemy.position));
    }
  }
  for (const HurtBeam& beam : enemy.zone.get().beams) {
    if (isActive(beam.activeIn, beam.window, enemy.current)) {
      hurts.beams.push_back(
          beamLineOf(beam, enemy.position, enemy.current->since));
    }
  }
}

bool hurtsBounds(const ActiveHurts& hurts, const engine::Bounds& ship) {
  return std::ranges::any_of(hurts.boxes,
                             [&ship](const engine::Bounds& box) noexcept {
                               return engine::overlaps(box, ship);
                             }) ||
         std::ranges::any_of(hurts.beams,
                             [&ship](const BeamLine& line) noexcept {
                               return beamOverlaps(line, ship);
                             });
}

}  // namespace rtype::game
