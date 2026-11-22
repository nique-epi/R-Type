#include "PaintedBeamTimeline.hpp"
#include <algorithm>
#include <chrono>
#include <iterator>
#include <optional>
#include "AnnouncedAction.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "PaintedBeams.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The time from the server tick @p fromTick to the server tick
 * @p toTick. */
engine::Duration timeBetweenTicks(double fromTick, double toTick) {
  return std::chrono::duration_cast<engine::Duration>(
      (toTick - fromTick) *
      std::chrono::duration<double, engine::Duration::period>(
          engine::SIMULATION_TICK_DURATION));
}

/** @brief Shows on @p beams the newest of its announced actions at a tick no
 * later than @p renderTick, and forgets it and the older ones; then counts
 * the time of the action shown up to @p renderTick. */
void showDue(PaintedBeams& beams, double renderTick) {
  const auto due = std::ranges::find_if(
      beams.announced, [renderTick](const AnnouncedAction& announced) {
        return announced.serverTick > renderTick;
      });
  if (due != beams.announced.begin()) {
    const AnnouncedAction& newest = *std::prev(due);
    beams.shown.key = newest.key;
    beams.shownTick = newest.serverTick;
    beams.announced.erase(beams.announced.begin(), due);
  }
  beams.shown.since = timeBetweenTicks(beams.shownTick, renderTick);
}

}  // namespace

void showPaintedBeams(engine::ComponentRegistry& components,
                      std::optional<double> renderTick) {
  if (!renderTick.has_value()) {
    return;
  }
  components.forEach<PaintedBeams, game::Position>(
      [&renderTick](engine::Entity, PaintedBeams& beams,
                    const game::Position&) { showDue(beams, *renderTick); });
}

}  // namespace rtype::client
