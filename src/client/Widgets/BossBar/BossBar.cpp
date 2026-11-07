#include "BossBar.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <optional>
#include "BossBarConstants.hpp"
#include "BossBarLook.hpp"
#include "BossColorConstants.hpp"
#include "BossFight.hpp"
#include "BossGaugeLook.hpp"
#include "BossGaugeState.hpp"
#include "BossStatus.hpp"
#include "BossTrackerConstants.hpp"
#include "Easing.hpp"
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The points of one gauge: those it holds, those it held before the
 * losses of the lag, and those it holds full. */
struct GaugePoints {
  int health{};
  int lag{};
  int share{};
};

/** @returns The columns @p points of @p share light, rounded up so a last
 * point still shows. */
int columnsOf(int points, int share) {
  if (points <= 0 || share <= 0) {
    return 0;
  }
  return std::min(BOSS_GAUGE_INNER_WIDTH,
                  ((BOSS_GAUGE_INNER_WIDTH * points) + share - 1) / share);
}

BossGaugeLook gaugeOf(GaugePoints points, BossGaugeState state) {
  const int fill = columnsOf(points.health, points.share);
  return BossGaugeLook{
      .fill = fill,
      .lagEnd = std::max(fill, columnsOf(points.lag, points.share)),
      .state = state};
}

int shownPoints(int points, float share) {
  return static_cast<int>(static_cast<float>(points) * share);
}

BossGaugeLook phaseOneOf(const BossFight& fight, int health, int lag) {
  const int lower = fight.fullHealth / BOSS_PHASE_COUNT;
  return gaugeOf(
      GaugePoints{.health = health - lower,
                  .lag = lag - lower,
                  .share = fight.fullHealth - lower},
      fight.health > lower ? BossGaugeState::Active : BossGaugeState::Spent);
}

BossGaugeLook phaseTwoOf(const BossFight& fight, int health, int lag) {
  const int lower = fight.fullHealth / BOSS_PHASE_COUNT;
  return gaugeOf(GaugePoints{.health = std::min(health, lower),
                             .lag = std::min(lag, lower),
                             .share = lower},
                 fight.stage == network::BossStage::Enraged
                     ? BossGaugeState::Enraged
                     : BossGaugeState::Waiting);
}

/** @returns The milliseconds into the shatter of gauge I, or nothing: none
 * when the gauge was already empty as the bar appeared. */
std::optional<int> shatterAt(const BossFight& fight, engine::Duration now) {
  if (!fight.phaseOneEmptiedAt.has_value() ||
      *fight.phaseOneEmptiedAt <= fight.fightingSince.value_or(now)) {
    return std::nullopt;
  }
  const engine::Duration into = now - *fight.phaseOneEmptiedAt;
  if (into >= BOSS_GAUGE_SHATTER_TIME) {
    return std::nullopt;
  }
  return static_cast<int>(
      std::chrono::duration_cast<std::chrono::milliseconds>(into).count());
}

sf::Color borderOf(BossGaugeState state) {
  switch (state) {
    case BossGaugeState::Active:
      return EDGE_BLUE;
    case BossGaugeState::Enraged:
      return RAGE_RED;
    case BossGaugeState::Spent:
      return BOSS_SPENT_MARK_COLOR;
    case BossGaugeState::Waiting:
      break;
  }
  return BOSS_MARK_COLOR;
}

sf::IntRect innerOf(sf::IntRect gauge) {
  const sf::Vector2i border(BOSS_GAUGE_BORDER, BOSS_GAUGE_BORDER);
  return {gauge.position + border, gauge.size - (border * SIDES_PER_AXIS)};
}

void paintPixel(IInterfaceCanvas& canvas, sf::Vector2i at, sf::Color color) {
  canvas.fill(sf::IntRect(at, {1, 1}), color);
}

void paintHatching(IInterfaceCanvas& canvas, sf::IntRect inner) {
  for (int column = 0; column < inner.size.x; ++column) {
    for (int row = 0; row < inner.size.y; ++row) {
      if ((column + row) % BOSS_HATCH_PERIOD == 0) {
        paintPixel(canvas, inner.position + sf::Vector2i(column, row),
                   BOSS_SPENT_MARK_COLOR);
      }
    }
  }
}

void paintGauge(IInterfaceCanvas& canvas, sf::IntRect area,
                BossGaugeLook gauge) {
  canvas.fill(area, borderOf(gauge.state));
  const sf::IntRect inner = innerOf(area);
  canvas.fill(inner, BOSS_EMPTY_COLOR);
  if (gauge.state == BossGaugeState::Spent) {
    paintHatching(canvas, inner);
    return;
  }
  if (gauge.lagEnd > 0) {
    canvas.fill(sf::IntRect(inner.position, {gauge.lagEnd, inner.size.y}),
                BOSS_LAG_COLOR);
  }
  if (gauge.fill > 0) {
    canvas.fill(
        sf::IntRect(inner.position, {gauge.fill, inner.size.y}),
        gauge.state == BossGaugeState::Enraged ? RAGE_RED : BYDO_EYE_PINK);
  }
}

void paintHotPixels(IInterfaceCanvas& canvas, int fill, int step) {
  if (fill <= 0) {
    return;
  }
  const sf::IntRect inner = innerOf(BOSS_PHASE_TWO_GAUGE);
  for (int pixel = 0; pixel < BOSS_HOT_PIXELS; ++pixel) {
    const int column =
        ((step * BOSS_HOT_STEP_STRIDE) + (pixel * BOSS_HOT_PIXEL_STRIDE)) %
        fill;
    const int row = (step + pixel) % inner.size.y;
    paintPixel(canvas, inner.position + sf::Vector2i(column, row), RAGE_HOT);
  }
}

void paintShards(IInterfaceCanvas& canvas, int milliseconds) {
  const sf::IntRect inner = innerOf(BOSS_PHASE_ONE_GAUGE);
  const float time =
      shareOf(std::chrono::milliseconds(milliseconds), BOSS_GAUGE_SHATTER_TIME);
  const int fall =
      static_cast<int>(static_cast<float>(BOSS_SHARD_FALL) * time * time);
  for (int shard = 0; shard < BOSS_GAUGE_SHARDS; ++shard) {
    const auto drift = static_cast<int>(
        static_cast<float>((shard % BOSS_SHARD_DRIFTS) -
                           (BOSS_SHARD_DRIFTS / BOSS_GAUGE_COUNT)) *
        BOSS_SHARD_DRIFT_PIXELS * time);
    const sf::Vector2i at(
        inner.position.x + ((shard * inner.size.x) / BOSS_GAUGE_SHARDS) + drift,
        inner.position.y + (shard % inner.size.y) + fall);
    paintPixel(canvas, at,
               shard % BOSS_GAUGE_COUNT == 0 ? BYDO_EYE_PINK : BOSS_LAG_COLOR);
  }
}

int centerOf(sf::IntRect gauge) {
  return gauge.position.x + (gauge.size.x / BOSS_GAUGE_COUNT);
}

void paintNames(IInterfaceCanvas& canvas) {
  canvas.write(BOSS_NAME_TEXT, {BOSS_BAR_CENTER_X, BOSS_NAME_TOP},
               TextStyle{.font = TextFont::Regular,
                         .color = BOSS_MARK_COLOR,
                         .outlined = true,
                         .alignment = TextAlignment::Center});
  const TextStyle label{.font = TextFont::Small,
                        .color = BOSS_MARK_COLOR,
                        .outlined = true,
                        .alignment = TextAlignment::Center};
  canvas.write(PHASE_TWO_LABEL,
               {centerOf(BOSS_PHASE_TWO_GAUGE), BOSS_LABEL_TOP}, label);
  canvas.write(PHASE_ONE_LABEL,
               {centerOf(BOSS_PHASE_ONE_GAUGE), BOSS_LABEL_TOP}, label);
}

}  // namespace

BossBarLook bossBarAt(const BossFight& fight, engine::Duration now,
                      FlashPreference preference) {
  if (!fight.fightingSince.has_value() ||
      fight.stage == network::BossStage::Defeated) {
    return {};
  }
  const float filled =
      progressBetween(now, *fight.fightingSince, BOSS_BAR_FILL_TIME);
  const int health = shownPoints(fight.health, filled);
  const bool lagShows =
      fight.lastLossAt.has_value() && now - *fight.lastLossAt < BOSS_LAG_TIME;
  const int lag = lagShows ? shownPoints(fight.lagHealth, filled) : health;
  const bool flickers = fight.stage == network::BossStage::Enraged &&
                        preference == FlashPreference::Full;
  return BossBarLook{
      .shown = true,
      .phaseOne = phaseOneOf(fight, health, lag),
      .phaseTwo = phaseTwoOf(fight, health, lag),
      .flickerStep =
          flickers ? static_cast<int>(now / BOSS_BAR_FLICKER_STEP) : 0,
      .shatterMilliseconds = shatterAt(fight, now)};
}

void paintBossBar(IInterfaceCanvas& canvas, const BossBarLook& look) {
  if (!look.shown) {
    return;
  }
  paintNames(canvas);
  paintGauge(canvas, BOSS_PHASE_TWO_GAUGE, look.phaseTwo);
  paintGauge(canvas, BOSS_PHASE_ONE_GAUGE, look.phaseOne);
  if (look.phaseTwo.state == BossGaugeState::Enraged) {
    paintHotPixels(canvas, look.phaseTwo.fill, look.flickerStep);
  }
  if (look.shatterMilliseconds.has_value()) {
    paintShards(canvas, *look.shatterMilliseconds);
  }
}

}  // namespace rtype::client
