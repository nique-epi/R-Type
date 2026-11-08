#include "WarningBanner.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <string_view>
#include "Easing.hpp"
#include "FlashPreference.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"
#include "WarningBannerConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief A hazard tape: its top row, how far it is from its place (to the
 * left when negative), and how far its stripes scrolled. */
struct HazardTape {
  int top{};
  int shift{};
  int scroll{};
};

/** @brief One row of stripes of a tape: its row, the columns it covers, and
 * the column a red stripe starts at, modulo HAZARD_STRIPE_PERIOD. */
struct StripeRow {
  int row{};
  int left{};
  int right{};
  int phase{};
};

int bandHalfHeightAt(engine::Duration sinceArrival) {
  const float opened = easeOutCubic(
      progressBetween(sinceArrival, WARNING_START, WARNING_BAND_OPENING));
  const float closed = easeInCubic(progressBetween(
      sinceArrival, WARNING_END - WARNING_BAND_CLOSING, WARNING_BAND_CLOSING));
  return pixelsOf(WARNING_BAND_HALF_HEIGHT, opened * (1.0F - closed));
}

int tapeShiftAt(engine::Duration intoWarning) {
  const float slid = easeOutCubic(progressBetween(
      intoWarning, engine::Duration::zero(), HAZARD_TAPE_SLIDE));
  return pixelsOf(ALERT_PLAYFIELD_AREA.size.x, 1.0F - slid);
}

int stripeScrollAt(engine::Duration intoWarning) {
  const auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(intoWarning)
          .count();
  return static_cast<int>(static_cast<float>(milliseconds) *
                          HAZARD_SCROLL_PIXELS_PER_MILLISECOND) %
         HAZARD_STRIPE_PERIOD;
}

int tearAt(engine::Duration intoWarning, FlashPreference flashes) {
  const auto step = static_cast<std::size_t>(intoWarning / WARNING_TEAR_STEP);
  if (flashes == FlashPreference::Reduced ||
      step >= WARNING_TEAR_OFFSETS.size()) {
    return 0;
  }
  return WARNING_TEAR_OFFSETS.at(step);
}

std::size_t wordLettersAt(engine::Duration intoWarning,
                          FlashPreference flashes) {
  const std::size_t typed = lettersTypedAt(
      intoWarning, WARNING_WORD_LETTER_TIME, WARNING_WORD.size());
  const engine::Duration intoBlinking =
      intoWarning -
      typingTimeOf(WARNING_WORD_LETTER_TIME, WARNING_WORD.size()) -
      typingTimeOf(WARNING_SUBTITLE_LETTER_TIME, WARNING_SUBTITLE.size());
  if (flashes == FlashPreference::Reduced ||
      intoBlinking < engine::Duration::zero()) {
    return typed;
  }
  return intoBlinking % WARNING_BLINK_PERIOD <
                 WARNING_BLINK_PERIOD - WARNING_BLINK_HIDDEN
             ? typed
             : 0;
}

/** @brief Paints the red stripes of one row of a tape, over its INK. */
void paintStripeRow(IInterfaceCanvas& canvas, const StripeRow& stripes) {
  const int offset = (((stripes.left - stripes.phase) % HAZARD_STRIPE_PERIOD) +
                      HAZARD_STRIPE_PERIOD) %
                     HAZARD_STRIPE_PERIOD;
  for (int start = stripes.left - offset; start < stripes.right;
       start += HAZARD_STRIPE_PERIOD) {
    const int left = std::max(start, stripes.left);
    const int right = std::min(start + HAZARD_STRIPE_WIDTH, stripes.right);
    if (right > left) {
      canvas.fill(sf::IntRect({left, stripes.row}, {right - left, 1}),
                  HAZARD_RED);
    }
  }
}

/** @brief Paints the part of a tape inside the playfield: INK, then the
 * stripes of each row between its edge rows, slanted one column per row. */
void paintTape(IInterfaceCanvas& canvas, HazardTape tape) {
  const int width = ALERT_PLAYFIELD_AREA.size.x;
  const int left = std::max(0, tape.shift);
  const int right = std::min(width, width + tape.shift);
  if (right <= left) {
    return;
  }
  canvas.fill(sf::IntRect({left, tape.top}, {right - left, HAZARD_TAPE_HEIGHT}),
              INK);
  for (int row = HAZARD_TAPE_EDGE_ROWS;
       row < HAZARD_TAPE_HEIGHT - HAZARD_TAPE_EDGE_ROWS; ++row) {
    paintStripeRow(canvas, StripeRow{.row = tape.top + row,
                                     .left = left,
                                     .right = right,
                                     .phase = tape.shift + tape.scroll + row});
  }
}

/** @returns The rows band @p band of a word whose line starts at row @p top
 * may paint: WARNING_WORD_SCALE rows, the first and the last one more
 * outwards for the outline. */
sf::IntRect bandClipOf(int top, std::size_t band) {
  const int above = band == 0 ? WARNING_OUTLINE_WIDTH : 0;
  const int below =
      band + 1 == WARNING_FIRE_BANDS.size() ? WARNING_OUTLINE_WIDTH : 0;
  return {{0, top + (static_cast<int>(band) * WARNING_WORD_SCALE) - above},
          {ALERT_PLAYFIELD_AREA.size.x, WARNING_WORD_SCALE + above + below}};
}

TextStyle wordStyleOf(sf::Color color, bool outlined,
                      std::optional<sf::IntRect> clip) {
  return TextStyle{.font = TextFont::Regular,
                   .color = color,
                   .outlined = outlined,
                   .alignment = TextAlignment::Left,
                   .scale = WARNING_WORD_SCALE,
                   .clip = clip};
}

/** @brief Darkens one row in WARNING_SCANLINE_STEP across the word. */
void paintScanlines(IInterfaceCanvas& canvas, sf::IntRect word) {
  for (int row = 1; row < word.size.y; row += WARNING_SCANLINE_STEP) {
    canvas.fill(
        sf::IntRect({word.position.x, word.position.y + row}, {word.size.x, 1}),
        WARNING_SCANLINE_COLOR);
  }
}

void paintWord(IInterfaceCanvas& canvas, const WarningBannerLook& look) {
  if (look.wordLetters == 0) {
    return;
  }
  const std::string_view shown = WARNING_WORD.substr(0, look.wordLetters);
  const int wholeWidth =
      canvas.textWidth(WARNING_WORD, TextFont::Regular) * WARNING_WORD_SCALE;
  const sf::Vector2i anchor(
      (ALERT_PLAYFIELD_AREA.size.x - wholeWidth) / SIDES_PER_AXIS,
      WARNING_WORD_TOP);
  canvas.write(
      shown,
      anchor + sf::Vector2i(WARNING_SHADOW_OFFSET, WARNING_SHADOW_OFFSET),
      wordStyleOf(WARNING_SHADOW_COLOR, false, std::nullopt));
  for (std::size_t band = 0; band < WARNING_FIRE_BANDS.size(); ++band) {
    const int tear =
        band % WARNING_TEAR_DIRECTIONS == 0 ? look.tear : -look.tear;
    canvas.write(shown, {anchor.x + tear, anchor.y},
                 wordStyleOf(WARNING_FIRE_BANDS.at(band), true,
                             bandClipOf(anchor.y, band)));
  }
  const int shownWidth =
      canvas.textWidth(shown, TextFont::Regular) * WARNING_WORD_SCALE;
  paintScanlines(
      canvas,
      sf::IntRect(anchor, {shownWidth, TEXT_LINE_HEIGHT * WARNING_WORD_SCALE}));
}

void paintSubtitle(IInterfaceCanvas& canvas, std::size_t letters) {
  if (letters == 0) {
    return;
  }
  const int width = canvas.textWidth(WARNING_SUBTITLE, TextFont::Small);
  canvas.write(WARNING_SUBTITLE.substr(0, letters),
               {(ALERT_PLAYFIELD_AREA.size.x - width) / SIDES_PER_AXIS,
                WARNING_SUBTITLE_TOP},
               TextStyle{.font = TextFont::Small,
                         .color = WARNING_SUBTITLE_COLOR,
                         .outlined = true,
                         .alignment = TextAlignment::Left});
}

}  // namespace

WarningBannerLook warningBannerAt(engine::Duration sinceArrival,
                                  FlashPreference flashes) {
  const engine::Duration intoWarning = sinceArrival - WARNING_START;
  if (intoWarning < engine::Duration::zero() || sinceArrival >= WARNING_END) {
    return {};
  }
  const bool closing = sinceArrival >= WARNING_END - WARNING_BAND_CLOSING;
  const engine::Duration intoSubtitle =
      intoWarning - typingTimeOf(WARNING_WORD_LETTER_TIME, WARNING_WORD.size());
  return WarningBannerLook{
      .bandHalfHeight = bandHalfHeightAt(sinceArrival),
      .tapeShift = tapeShiftAt(intoWarning),
      .stripeScroll = stripeScrollAt(intoWarning),
      .wordLetters = closing ? 0 : wordLettersAt(intoWarning, flashes),
      .subtitleLetters =
          closing ? 0
                  : lettersTypedAt(intoSubtitle, WARNING_SUBTITLE_LETTER_TIME,
                                   WARNING_SUBTITLE.size()),
      .tear = tearAt(intoWarning, flashes)};
}

void paintWarningBanner(IInterfaceCanvas& canvas,
                        const WarningBannerLook& look) {
  if (look == WarningBannerLook{}) {
    return;
  }
  const int top = WARNING_BAND_MIDDLE_ROW - look.bandHalfHeight;
  const int bottom = WARNING_BAND_MIDDLE_ROW + look.bandHalfHeight;
  if (bottom > top) {
    canvas.fill(
        sf::IntRect({0, top}, {ALERT_PLAYFIELD_AREA.size.x, bottom - top}),
        WARNING_BAND_COLOR);
  }
  paintTape(canvas, HazardTape{.top = top - HAZARD_TAPE_HEIGHT,
                               .shift = -look.tapeShift,
                               .scroll = look.stripeScroll});
  paintTape(canvas, HazardTape{.top = bottom,
                               .shift = look.tapeShift,
                               .scroll = -look.stripeScroll});
  paintWord(canvas, look);
  paintSubtitle(canvas, look.subtitleLetters);
}

}  // namespace rtype::client
