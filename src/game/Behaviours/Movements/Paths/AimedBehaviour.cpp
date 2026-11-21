#include "AimedBehaviour.hpp"
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include "AimedBehaviourConstants.hpp"
#include "AimedHeading.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "UnitHeading.hpp"

namespace rtype::game {

namespace {

/** @returns The heading towards the nearest ship of @p view, as a share of the
 * full speed on each axis whose length is 1; LEFTWARD when there is no ship,
 * or when the ship is not left of the body. */
Direction headingTowardsTheShip(const BehaviourView& view) {
  if (!view.nearestShip.has_value() || view.nearestShip->x >= view.position.x) {
    return LEFTWARD;
  }
  return headingAlong(Direction{.x = view.nearestShip->x - view.position.x,
                                .y = view.nearestShip->y - view.position.y});
}

/** @returns The heading the body took on its first tick, nothing before that
 * tick or when the behaviour has nowhere to keep it. */
std::optional<Direction> keptHeadingOf(const BehaviourView& view) {
  if (view.behaviourState == nullptr) {
    return std::nullopt;
  }
  const auto* kept = dynamic_cast<AimedHeading*>(view.behaviourState->get());
  if (kept == nullptr) {
    return std::nullopt;
  }
  return kept->heading();
}

/** @brief Keeps @p heading as the heading of the body of @p view for the rest
 * of its flight, when the behaviour has somewhere to keep it. */
void keepHeading(const BehaviourView& view, Direction heading) {
  if (view.behaviourState != nullptr) {
    *view.behaviourState = std::make_shared<AimedHeading>(heading);
  }
}

}  // namespace

std::string_view AimedBehaviour::name() const { return AIMED_BEHAVIOUR_NAME; }

std::span<const BehaviourSetting> AimedBehaviour::settings() const {
  return {};
}

Intent AimedBehaviour::steer(const BehaviourView& view) const {
  std::optional<Direction> heading = keptHeadingOf(view);
  if (!heading.has_value()) {
    heading = headingTowardsTheShip(view);
    keepHeading(view, *heading);
  }
  return Intent{.direction = heading.value_or(LEFTWARD),
                .shooting = true,
                .steered = true};
}

}  // namespace rtype::game
