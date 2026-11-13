#include "RulePilot.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>
#include <optional>
#include <vector>
#include "ActionChoice.hpp"
#include "ActionChoiceConstants.hpp"
#include "ActionSet.hpp"
#include "BotSight.hpp"
#include "EntityKind.hpp"
#include "PlayerAction.hpp"
#include "PlayfieldConstants.hpp"
#include "RulePilotConstants.hpp"
#include "SeenEntity.hpp"
#include "ShipControlConstants.hpp"
#include "SightFigures.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

namespace {

using client::ActionSet;
using client::PlayerAction;

/** @brief Where a ship is and how fast it goes on each axis. */
struct ShipCourse {
  float x{};
  float y{};
  float velocityX{};
  float velocityY{};
};

/** @returns The course of a ship at @p figures holding @p held. */
ShipCourse courseOf(const SightFigures& figures, const ActionSet& held) {
  const int across = static_cast<int>(held.has(PlayerAction::Right)) -
                     static_cast<int>(held.has(PlayerAction::Left));
  const int along = static_cast<int>(held.has(PlayerAction::Down)) -
                    static_cast<int>(held.has(PlayerAction::Up));
  const float speed = (across != 0 && along != 0)
                          ? game::SHIP_SPEED / std::numbers::sqrt2_v<float>
                          : game::SHIP_SPEED;
  return ShipCourse{.x = figures.shipX,
                    .y = figures.shipY,
                    .velocityX = static_cast<float>(across) * speed,
                    .velocityY = static_cast<float>(along) * speed};
}

/** @returns @p course after @p seconds, kept inside the play area. */
ShipCourse courseAfter(ShipCourse course, float seconds) {
  const float halfWidth = game::SHIP_WIDTH / 2.0F;
  const float halfHeight = game::SHIP_HEIGHT / 2.0F;
  course.x = std::clamp(course.x + (course.velocityX * seconds), halfWidth,
                        game::PLAYFIELD_WIDTH - halfWidth);
  course.y = std::clamp(course.y + (course.velocityY * seconds), halfHeight,
                        game::PLAY_AREA_HEIGHT - halfHeight);
  return course;
}

/** @returns Whether @p entity, @p seconds from now, touches a ship at
 * @p ship. */
bool touches(const SeenEntity& entity, const ShipCourse& ship, float seconds) {
  const float reachX =
      ((entity.width + game::SHIP_WIDTH) / 2.0F) + RULE_DANGER_MARGIN;
  const float reachY =
      ((entity.height + game::SHIP_HEIGHT) / 2.0F) + RULE_DANGER_MARGIN;
  return std::abs(entity.x + (entity.velocityX * seconds) - ship.x) < reachX &&
         std::abs(entity.y + (entity.velocityY * seconds) - ship.y) < reachY;
}

/** @returns What @p entity costs a ship on @p course: the sooner it touches
 * it, the more. */
float touchCost(const SeenEntity& entity, const ShipCourse& course) {
  if (entity.kind == EntityKind::Item) {
    return 0.0F;
  }
  for (std::size_t step = 1; step <= RULE_LOOK_AHEAD_STEPS; ++step) {
    const float seconds = static_cast<float>(step) * RULE_LOOK_AHEAD_STEP;
    if (touches(entity, courseAfter(course, seconds), seconds)) {
      return RULE_TOUCH_COST *
             static_cast<float>(RULE_LOOK_AHEAD_STEPS + 1 - step);
    }
  }
  return 0.0F;
}

/** @returns The boss among the enemies of @p sight, if one is there. */
std::optional<SeenEntity> bossOf(const BotSight& sight) {
  for (const SeenEntity& entity : sight.entities()) {
    if (entity.kind == EntityKind::Enemy &&
        entity.width >= RULE_BOSS_SMALLEST_WIDTH) {
      return entity;
    }
  }
  return std::nullopt;
}

/** @returns What bot @p botIndex aims at: the boss, or its share of the
 * enemies ahead of its ship, in the order of their ids. */
std::optional<SeenEntity> targetOf(const BotSight& sight,
                                   std::size_t botIndex) {
  if (const std::optional<SeenEntity> boss = bossOf(sight)) {
    return boss;
  }
  std::vector<SeenEntity> ahead;
  for (const SeenEntity& entity : sight.entities()) {
    if (entity.kind == EntityKind::Enemy && entity.x > sight.figures().shipX) {
      ahead.push_back(entity);
    }
  }
  if (ahead.empty()) {
    return std::nullopt;
  }
  std::ranges::sort(ahead, {}, &SeenEntity::entityId);
  return ahead[botIndex % ahead.size()];
}

/** @returns What a ship next on @p next costs for being away from its row and
 * its place. */
float placeCost(const ShipCourse& next,
                const std::optional<SeenEntity>& target) {
  const float home = std::abs(next.x - RULE_HOME_X) * RULE_HOME_COST;
  if (!target.has_value()) {
    return home;
  }
  return home +
         std::max(0.0F, std::abs(target->y - next.y) - RULE_ALIGN_TOLERANCE);
}

/** @returns What the way to move @p held costs for @p sight. */
float costOf(const ActionSet& held, const BotSight& sight,
             const std::optional<SeenEntity>& target) {
  const ShipCourse course = courseOf(sight.figures(), held);
  float cost = placeCost(courseAfter(course, RULE_LOOK_AHEAD_STEP), target);
  for (const SeenEntity& entity : sight.entities()) {
    cost += touchCost(entity, course);
  }
  return cost;
}

}  // namespace

RulePilot::RulePilot(std::size_t botIndex) : botIndex_(botIndex) {}

ActionSet RulePilot::advance([[maybe_unused]] engine::Duration elapsed,
                             const BotSight& sight) {
  if (!sight.figures().inPlay) {
    return ActionSet{};
  }
  const std::optional<SeenEntity> target = targetOf(sight, botIndex_);
  std::size_t best = 0;
  float lowest = std::numeric_limits<float>::max();
  for (std::size_t choice = 0; choice < DIRECTION_CHOICES.size(); ++choice) {
    const float cost = costOf(actionSetOf(choice), sight, target);
    if (cost < lowest) {
      lowest = cost;
      best = choice;
    }
  }
  return actionSetOf(best + DIRECTION_CHOICES.size());
}

}  // namespace rtype::tools
