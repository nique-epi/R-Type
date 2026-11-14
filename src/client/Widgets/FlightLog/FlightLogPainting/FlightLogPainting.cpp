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
#include "KopecksLabel.hpp"
#include "KopecksLabelConstants.hpp"
#include "KopecksPainting.hpp"
#include "MissionLogPage.hpp"
#include "MotionPreference.hpp"
#include "RankingPage.hpp"
#include "TauntLabel.hpp"
#include "TextMetrics.hpp"
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

/** @brief Paints, once the ranking replaces the mission log on the wide
 * page, the blank paper of the turning page over it, narrowing from the
 * right to the left over PAGE_TURN_TIME, its edge drawn; nothing with reduced
 * motion. */
void paintPageTurn(IInterfaceCanvas& canvas, const FlightLogLook& look,
                   sf::Vector2i page) {
  const float share =
      progressBetween(look.sinceStart, look.schedule.rowsStart, PAGE_TURN_TIME);
  if (look.motion == MotionPreference::Reduced || share >= 1.0F) {
    return;
  }
  const int covered = pixelsOf(WIDE_PAGE_WIDTH, 1.0F - easeOutCubic(share));
  canvas.fill(sf::IntRect(page, {covered, PAGE_SIZE.y}), PAPER_COLOR);
  canvas.fill(sf::IntRect(page + sf::Vector2i(covered, 0), {1, PAGE_SIZE.y}),
              PAPER_FAINT_COLOR);
}

/** @brief Paints the pages of the open book: the mission log on the left
 * page and the ranking on the right one, as drawn at the size the fonts are
 * drawn at; above it, one wide page over both, the mission log until the
 * rows start, then the ranking, the page turning to it. */
void paintOpenPages(IInterfaceCanvas& canvas, const FlightLogLook& look,
                    BookPose pose) {
  const sf::Vector2i left(pose.hinge - PAGE_INSET.x - PAGE_SIZE.x,
                          pose.top + PAGE_INSET.y);
  if (canvas.textMetrics() == TextMetrics{}) {
    paintMissionLogPage(canvas, look, left);
    paintRankingPage(canvas, look,
                     {pose.hinge + PAGE_INSET.x, pose.top + PAGE_INSET.y});
    return;
  }
  canvas.fill(sf::IntRect(left, {WIDE_PAGE_WIDTH, PAGE_SIZE.y}), PAPER_COLOR);
  if (look.sinceStart < look.schedule.rowsStart) {
    paintMissionLogPage(canvas, look, left);
    return;
  }
  paintRankingPage(canvas, look, left);
  paintPageTurn(canvas, look, left);
}

/** @brief Writes what the game earned the player, from KOPECKS_GAIN_TOP_LEFT
 * rightwards, once the call to play again starts; nothing for no gain. */
void paintKopecksGain(IInterfaceCanvas& canvas, const FlightLogLook& look) {
  if (look.kopecksGained == 0 || look.sinceStart < look.schedule.promptStart) {
    return;
  }
  KopecksLabel gain{.topRight = KOPECKS_GAIN_TOP_LEFT,
                    .amount = look.kopecksGained,
                    .prefix = KOPECKS_GAIN_PREFIX,
                    .outlined = true};
  gain.topRight.x += kopecksLabelWidth(gain, canvas.textMetrics());
  paintKopecks(canvas, gain);
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
    paintOpenPages(canvas, look, pose);
  }
  paintTauntLabel(canvas, look, pose);
  paintDust(canvas, look);
  paintEndChoices(canvas, look);
  paintKopecksGain(canvas, look);
}

}  // namespace rtype::client
