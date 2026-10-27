#include "FlightLogPainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>
#include <numbers>
#include "BookMotion.hpp"
#include "BookPainting.hpp"
#include "BookPose.hpp"
#include "DustGrain.hpp"
#include "Easing.hpp"
#include "EndChoices.hpp"
#include "FlashPreference.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogTimingConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "MissionLogPage.hpp"
#include "MotionPreference.hpp"
#include "RankingPage.hpp"
#include "TauntLabel.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How far the stamp shakes the book now: SHAKE_OFFSET one way then
 * the other every SHAKE_STEP for SHAKE_TIME; nothing when motion or flashes
 * are reduced. */
sf::Vector2i shakeOf(const FlightLogLook& look) {
  const engine::Duration age = look.sinceStart - look.schedule.stamped;
  if (look.motion == MotionPreference::Reduced ||
      look.flashes == FlashPreference::Reduced ||
      age < engine::Duration::zero() || age >= SHAKE_TIME) {
    return {};
  }
  const bool odd = (age / SHAKE_STEP) % SHAKE_SIDES != 0;
  return odd ? sf::Vector2i(SHAKE_OFFSET.x, -SHAKE_OFFSET.y)
             : sf::Vector2i(-SHAKE_OFFSET.x, SHAKE_OFFSET.y);
}

void paintRibbon(IInterfaceCanvas& canvas, BookPose pose) {
  const sf::Vector2i topLeft =
      sf::Vector2i(pose.hinge, pose.top) + RIBBON_OFFSET;
  canvas.fill(sf::IntRect(topLeft, RIBBON_SIZE), RIBBON_COLOR);
  canvas.fill(sf::IntRect(topLeft, {1, RIBBON_SIZE.y}), RIBBON_SHADE_COLOR);
  for (const sf::IntRect end : RIBBON_FORK) {
    canvas.fill(sf::IntRect(topLeft + end.position, end.size), RIBBON_COLOR);
  }
  canvas.fill(sf::IntRect(topLeft + RIBBON_NOTCH.position, RIBBON_NOTCH.size),
              RIBBON_NOTCH_COLOR);
}

/** @returns Where a grain of the dust lies at @p share of its flight: out
 * from its bottom corner of the closed book, risen then fallen back. */
sf::Vector2i grainAt(const DustGrain& grain, float share) {
  const int bottom = BOOK_TOP + BOOK_HALF_SIZE.y + BOOK_SIDE_DEPTH;
  const int corner = grain.side < 0
                         ? CLOSED_HINGE
                         : CLOSED_HINGE + BOOK_HALF_SIZE.x + BOOK_SIDE_DEPTH;
  return {
      corner + pixelsOf(grain.side * grain.reach, easeOutCubic(share)),
      bottom - grain.row -
          pixelsOf(grain.rise, std::sin(share * std::numbers::pi_v<float>))};
}

/** @brief Paints the dust of the landing: each grain flies out from a bottom
 * corner of the closed book and rises, large then small, every other one
 * gone at the end; none with reduced motion. */
void paintDust(IInterfaceCanvas& canvas, const FlightLogLook& look) {
  const float share = progressBetween(look.sinceStart, LANDING, DUST_TIME);
  if (look.motion == MotionPreference::Reduced || share <= 0.0F ||
      share >= 1.0F) {
    return;
  }
  const int size = share < GRAIN_SHRINK_SHARE ? LARGE_GRAIN : 1;
  for (std::size_t index = 0; index < DUST_GRAINS.size(); ++index) {
    if (share > GRAIN_THIN_SHARE && index % DUST_COLORS.size() != 0) {
      continue;
    }
    canvas.fill(sf::IntRect(grainAt(DUST_GRAINS[index], share), {size, size}),
                DUST_COLORS.at(index % DUST_COLORS.size()));
  }
}

}  // namespace

void paintFlightLog(IInterfaceCanvas& canvas, const FlightLogLook& look) {
  const BookPose resting =
      bookPoseAt(look.sinceStart, look.schedule, look.exit);
  const sf::Vector2i shake = shakeOf(look);
  const BookPose pose{.hinge = resting.hinge + shake.x,
                      .top = resting.top + shake.y,
                      .open = resting.open};
  paintBook(canvas, pose);
  if (pose.open >= 1.0F && !look.exit.choice.has_value()) {
    paintRibbon(canvas, pose);
    paintMissionLogPage(
        canvas, look,
        {pose.hinge - PAGE_INSET.x - PAGE_SIZE.x, pose.top + PAGE_INSET.y});
    paintRankingPage(canvas, look,
                     {pose.hinge + PAGE_INSET.x, pose.top + PAGE_INSET.y});
  }
  paintTauntLabel(canvas, look, pose);
  paintDust(canvas, look);
  paintEndChoices(canvas, look);
}

}  // namespace rtype::client
