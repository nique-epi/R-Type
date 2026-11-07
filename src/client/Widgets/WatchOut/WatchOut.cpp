#include "WatchOut.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WarningBannerConstants.hpp"
#include "WatchOutConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How far the dashes scrolled @p sinceSignal after the signal,
 * modulo WATCH_OUT_DASH_PERIOD. */
int scrollAt(engine::Duration sinceSignal) {
  const auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(sinceSignal)
          .count();
  return static_cast<int>(static_cast<float>(milliseconds) *
                          WATCH_OUT_SCROLL_PIXELS_PER_MILLISECOND) %
         WATCH_OUT_DASH_PERIOD;
}

void paintLane(IInterfaceCanvas& canvas, const WatchOutLook& look) {
  const int width = ALERT_PLAYFIELD_AREA.size.x;
  for (int start = -look.scroll; start < width;
       start += WATCH_OUT_DASH_PERIOD) {
    const int left = std::max(start, 0);
    const int right = std::min(start + WATCH_OUT_DASH, width);
    if (right > left) {
      canvas.fill(sf::IntRect({left, look.row}, {right - left, 1}),
                  WATCH_OUT_LANE_COLOR);
    }
  }
}

}  // namespace

WatchOutLook watchOutAt(const WatchOutCue& cue, FlashPreference flashes) {
  if (!cue.sinceSignal.has_value() ||
      *cue.sinceSignal < engine::Duration::zero() ||
      *cue.sinceSignal >= WATCH_OUT_TIME) {
    return {};
  }
  const bool blinkOn = (*cue.sinceSignal / WATCH_OUT_BLINK_STEP) % 2 == 0;
  return WatchOutLook{.shown = true,
                      .row = std::clamp(cue.row, WATCH_OUT_TEXT_RISE,
                                        ALERT_PLAYFIELD_AREA.size.y - 1),
                      .lit = flashes == FlashPreference::Reduced || blinkOn,
                      .scroll = scrollAt(*cue.sinceSignal)};
}

void paintWatchOut(IInterfaceCanvas& canvas, const WatchOutLook& look) {
  if (!look.shown) {
    return;
  }
  paintLane(canvas, look);
  if (look.lit) {
    canvas.write(WATCH_OUT_TEXT,
                 {WATCH_OUT_TEXT_LEFT, look.row - WATCH_OUT_TEXT_RISE},
                 TextStyle{.font = TextFont::Small,
                           .color = WATCH_OUT_TEXT_COLOR,
                           .outlined = true,
                           .alignment = TextAlignment::Left});
  }
}

}  // namespace rtype::client
