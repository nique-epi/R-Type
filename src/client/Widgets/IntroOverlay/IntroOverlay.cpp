#include "IntroOverlay.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "Easing.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "IntroOverlayConstants.hpp"
#include "IntroOverlayLook.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "PlayerColors.hpp"
#include "PlayerPalette.hpp"
#include "RenderingConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief How a text is typed: from @c start, one letter every
 * @c letterTime, then erased letter by letter over @c eraseTime from
 * @c eraseStart, never erased by default. */
struct Typing {
  float start{};
  float letterTime{};
  float eraseStart = std::numeric_limits<float>::infinity();
  float eraseTime{};
};

/** @brief The logo in flight: its texture, its left column, and how far
 * through its slam it is, from 0 to 1. */
struct LogoFlight {
  TextureArea logo;
  float left{};
  float through{};
};

/** @returns The letters of @p text shown at @p real: none before the start,
 * one more every letter time, then one fewer each share of the erase. */
std::string_view typedPart(std::string_view text, float real,
                           const Typing& typing) {
  if (real < typing.start || text.empty()) {
    return {};
  }
  std::size_t count = std::min(
      text.size(),
      static_cast<std::size_t>((real - typing.start) / typing.letterTime) + 1);
  if (real >= typing.eraseStart) {
    const float letterGone = typing.eraseTime / static_cast<float>(text.size());
    const std::size_t gone =
        static_cast<std::size_t>((real - typing.eraseStart) / letterGone) + 1;
    count = std::min(count, text.size() - std::min(gone, text.size()));
  }
  return text.substr(0, count);
}

/** @returns How far the bands opened, from 0 to 1: slowly first. */
float bandOpeningAt(float real) {
  return easeInCubic(progressThrough(real, INTRO_SLAM, INTRO_BAND_OPENING));
}

/** @brief Paints the two bands of the cinema format, sliding out as they
 * open. */
void paintBands(IInterfaceCanvas& canvas, float real) {
  const int shown =
      INTRO_BAND_HEIGHT -
      static_cast<int>(std::round(static_cast<float>(INTRO_BAND_HEIGHT) *
                                  bandOpeningAt(real)));
  if (shown <= 0) {
    return;
  }
  canvas.fill(sf::IntRect({0, 0}, {INTERFACE_AREA.size.x, shown}), INK);
  canvas.fill(sf::IntRect({0, INTERFACE_AREA.size.y - shown},
                          {INTERFACE_AREA.size.x, shown}),
              INK);
}

/** @brief Writes the credit during the first beat. */
void paintCredit(IInterfaceCanvas& canvas, float real) {
  const std::string_view shown =
      typedPart(INTRO_CREDIT, real,
                Typing{.start = INTRO_CREDIT_START,
                       .letterTime = INTRO_CREDIT_LETTER_TIME,
                       .eraseStart = INTRO_CREDIT_ERASE_START,
                       .eraseTime = INTRO_CREDIT_ERASE_TIME});
  if (real < INTRO_BEAT && !shown.empty()) {
    canvas.write(shown, {SAFE_MARGIN, INTRO_BAND_TEXT_TOP}, INTRO_CREDIT_STYLE);
  }
}

/** @brief Writes the nickname of the pilot whose close-up plays, underlined
 * in its color as far as its letters go. */
void paintNameCards(IInterfaceCanvas& canvas, float real,
                    const PlayerPalette& palette) {
  for (std::size_t pilot = 0; pilot < INTRO_PILOT_NAMES.size(); ++pilot) {
    const float appear = closeupStartOf(pilot) + INTRO_NAME_DELAY;
    if (real < appear || real >= closeupStartOf(pilot) + INTRO_BEAT) {
      continue;
    }
    const std::string_view shown = typedPart(
        INTRO_PILOT_NAMES.at(pilot), real,
        Typing{.start = appear, .letterTime = INTRO_NAME_LETTER_TIME});
    const PlayerColors& colors = palette.at(pilot);
    canvas.write(shown, {SAFE_MARGIN, INTRO_BAND_TEXT_TOP},
                 TextStyle{.font = TextFont::Regular, .color = colors.light});
    canvas.fill(
        sf::IntRect({SAFE_MARGIN, INTRO_BAND_TEXT_TOP + INTRO_UNDERLINE_GAP},
                    {static_cast<int>(shown.size()) * REGULAR_FONT_ADVANCE, 1}),
        colors.ship);
  }
}

/** @brief Lays the three copies that trail the logo, the farthest the
 * darkest. */
void paintLogoTrail(IInterfaceCanvas& canvas, const LogoFlight& flight) {
  for (std::size_t copy = 0; copy < INTRO_LOGO_TRAIL.size(); ++copy) {
    const float behind = static_cast<float>(INTRO_LOGO_TRAIL.size() - copy) *
                         INTRO_TRAIL_SPACING * (1.0F - flight.through);
    canvas.drawImage(
        flight.logo,
        ImagePlacement{
            .topLeft = {static_cast<int>(std::round(flight.left + behind)),
                        INTRO_LOGO_PLACE.y},
            .tint = INTRO_LOGO_TRAIL.at(copy)},
        INTERFACE_AREA);
  }
}

/** @brief Paints the two lines that reach out of the sides of the logo once
 * it landed, along its middle row. */
void paintLogoLines(IInterfaceCanvas& canvas, float real, TextureArea logo) {
  const float since = real - INTRO_SLAM - INTRO_SLAM_TIME;
  if (since < 0.0F || since >= INTRO_LINES_SHOWN) {
    return;
  }
  const float reach = easeOutCubic(since / INTRO_LINES_REACH_TIME);
  const sf::Vector2i size = logo.rectangle.size;
  const int middle =
      INTRO_LOGO_PLACE.y + static_cast<int>(static_cast<float>(size.y) * HALF);
  const int leftReach = static_cast<int>(
      std::round(reach * static_cast<float>(INTRO_LOGO_PLACE.x)));
  const int right = INTRO_LOGO_PLACE.x + size.x;
  const int rightReach = static_cast<int>(
      std::round(reach * static_cast<float>(INTERFACE_AREA.size.x - right)));
  canvas.fill(
      sf::IntRect({INTRO_LOGO_PLACE.x - leftReach, middle}, {leftReach, 1}),
      EDGE_BLUE);
  canvas.fill(sf::IntRect({right, middle}, {rightReach, 1}), EDGE_BLUE);
}

/** @brief Paints the logo slamming in from the right edge to its place,
 * past it a little, with its trail, then its lines. */
void paintLogoSlam(IInterfaceCanvas& canvas, float real, TextureArea logo) {
  if (real < INTRO_SLAM) {
    return;
  }
  const float through = progressThrough(real, INTRO_SLAM, INTRO_SLAM_TIME);
  const float left =
      std::lerp(static_cast<float>(INTERFACE_AREA.size.x),
                static_cast<float>(INTRO_LOGO_PLACE.x), easeOutBack(through));
  if (through < 1.0F) {
    paintLogoTrail(canvas,
                   LogoFlight{.logo = logo, .left = left, .through = through});
  }
  canvas.drawImage(
      logo,
      ImagePlacement{
          .topLeft = {static_cast<int>(std::round(left)), INTRO_LOGO_PLACE.y}},
      INTERFACE_AREA);
  paintLogoLines(canvas, real, logo);
}

/** @brief Writes PRESS ANY KEY, on the bottom band while it shows, at the
 * footer once it opened. */
void paintHint(IInterfaceCanvas& canvas, float real) {
  if (real < INTRO_HINT_START) {
    return;
  }
  const int top = bandOpeningAt(real) < 1.0F ? INTRO_BAND_TEXT_TOP : FOOTER_TOP;
  canvas.write(INTRO_HINT, {INTERFACE_AREA.size.x - SAFE_MARGIN, top},
               INTRO_HINT_STYLE);
}

/** @brief Covers the whole picture with ink before time 0, then with less
 * and less of it until INTRO_FADE_IN_TIME: the black second and the fade
 * in. */
void paintFade(IInterfaceCanvas& canvas, float real) {
  const float covered = 1.0F - progressThrough(real, 0.0F, INTRO_FADE_IN_TIME);
  if (covered <= 0.0F) {
    return;
  }
  sf::Color ink = INK;
  ink.a = static_cast<std::uint8_t>(
      std::round(static_cast<float>(INK.a) * covered));
  canvas.fill(INTERFACE_AREA, ink);
}

}  // namespace

void paintIntroOverlay(IInterfaceCanvas& canvas, float real,
                       IntroOverlayLook look) {
  paintBands(canvas, real);
  paintCredit(canvas, real);
  paintNameCards(canvas, real, look.palette);
  paintLogoSlam(canvas, real, look.logo);
  paintHint(canvas, real);
  paintFade(canvas, real);
}

}  // namespace rtype::client
