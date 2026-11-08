#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "FireColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "StatusBandConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The playfield of the interface picture, above the status band: the
 * alert of a boss never paints the band. */
constexpr sf::IntRect ALERT_PLAYFIELD_AREA(
    {0, 0}, {INTERFACE_AREA.size.x, INTERFACE_AREA.size.y - STATUS_BAND_ROWS});

/** @brief The WARNING screen shows from WARNING_START to WARNING_END after
 * the Arriving status of the boss; before it, the music is silent. */
constexpr engine::Duration WARNING_START = std::chrono::milliseconds(600);
constexpr engine::Duration WARNING_END = std::chrono::milliseconds(3800);

/** @brief The dark band: the sky at 75 %, WARNING_BAND_HALF_HEIGHT rows on
 * each side of WARNING_BAND_MIDDLE_ROW once open; it opens in
 * WARNING_BAND_OPENING and closes in the last WARNING_BAND_CLOSING. */
constexpr int WARNING_BAND_MIDDLE_ROW = 114;
constexpr int WARNING_BAND_HALF_HEIGHT = 30;
constexpr engine::Duration WARNING_BAND_OPENING =
    std::chrono::milliseconds(350);
constexpr engine::Duration WARNING_BAND_CLOSING =
    std::chrono::milliseconds(400);
constexpr std::uint8_t WARNING_BAND_ALPHA = 191;
constexpr sf::Color WARNING_BAND_COLOR(SKY_COLOR.r, SKY_COLOR.g, SKY_COLOR.b,
                                       WARNING_BAND_ALPHA);

/** @brief The two hazard tapes along the band: HAZARD_TAPE_HEIGHT rows, an
 * edge row of INK at the top and the bottom, diagonal stripes
 * HAZARD_STRIPE_WIDTH wide alternating HAZARD_RED and INK; the upper one
 * slides in from the left, the lower one from the right, in
 * HAZARD_TAPE_SLIDE, their stripes scrolling opposite ways. */
constexpr sf::Color HAZARD_RED(255, 61, 46);
constexpr int HAZARD_TAPE_HEIGHT = 10;
constexpr int HAZARD_TAPE_EDGE_ROWS = 1;
constexpr int HAZARD_STRIPE_WIDTH = 6;
constexpr int HAZARD_STRIPE_COLORS = 2;
constexpr int HAZARD_STRIPE_PERIOD = HAZARD_STRIPE_WIDTH * HAZARD_STRIPE_COLORS;
constexpr engine::Duration HAZARD_TAPE_SLIDE = WARNING_BAND_OPENING;
constexpr float HAZARD_SCROLL_PIXELS_PER_MILLISECOND = 0.06F;

static_assert(WARNING_BAND_MIDDLE_ROW + WARNING_BAND_HALF_HEIGHT +
                      HAZARD_TAPE_HEIGHT <=
                  ALERT_PLAYFIELD_AREA.size.y,
              "the lower tape never reaches the status band");
static_assert(WARNING_BAND_MIDDLE_ROW - WARNING_BAND_HALF_HEIGHT -
                      HAZARD_TAPE_HEIGHT >=
                  0,
              "the upper tape stays inside the picture");

/** @brief The word, in Press Start 2P WARNING_WORD_SCALE times, one more
 * letter every WARNING_WORD_LETTER_TIME from WARNING_START; over a shadow
 * WARNING_SHADOW_OFFSET pixels down and right; written in one band per row
 * of the font, from yellow to red, each band torn sideways the other way from
 * the one above for the first WARNING_TEAR_STEP times
 * WARNING_TEAR_OFFSETS.size(); one row in WARNING_SCANLINE_STEP darkened. */
constexpr std::string_view WARNING_WORD = "WARNING";
constexpr int WARNING_WORD_SCALE = 4;
constexpr engine::Duration WARNING_WORD_LETTER_TIME =
    std::chrono::milliseconds(60);
constexpr int WARNING_WORD_TOP = 92;
constexpr int WARNING_SHADOW_OFFSET = 2;
constexpr sf::Color WARNING_SHADOW_COLOR = INK;
/** @brief The outline TextWriter draws is one pixel wide, at any scale. */
constexpr int WARNING_OUTLINE_WIDTH = 1;
constexpr std::array WARNING_FIRE_BANDS{FIRE_YELLOW, FIRE_YELLOW, FIRE_GOLD,
                                        FIRE_GOLD,   FIRE_RED,    FIRE_RED,
                                        HAZARD_RED,  HAZARD_RED};
static_assert(WARNING_FIRE_BANDS.size() == TEXT_CHARACTER_SIZE,
              "one band per row of the font");
constexpr int WARNING_SCANLINE_STEP = 2;
constexpr std::uint8_t WARNING_SCANLINE_ALPHA = 96;
constexpr sf::Color WARNING_SCANLINE_COLOR(INK.r, INK.g, INK.b,
                                           WARNING_SCANLINE_ALPHA);
constexpr engine::Duration WARNING_TEAR_STEP = std::chrono::milliseconds(50);
constexpr std::array WARNING_TEAR_OFFSETS{4, -3, 3, -2, 2, -2, 1, -1, 1, 0};
constexpr std::size_t WARNING_TEAR_DIRECTIONS = 2;

/** @brief The line under the word, in Silkscreen, one more letter every
 * WARNING_SUBTITLE_LETTER_TIME once the word is written. */
constexpr std::string_view WARNING_SUBTITLE =
    "A HUGE BYDO LIFE FORM IS APPROACHING FAST";
constexpr engine::Duration WARNING_SUBTITLE_LETTER_TIME =
    std::chrono::milliseconds(30);
constexpr int WARNING_SUBTITLE_TOP = 128;
constexpr sf::Color WARNING_SUBTITLE_COLOR = WARNING_ORANGE;

/** @brief Once both lines are written, the word hides WARNING_BLINK_HIDDEN
 * of every WARNING_BLINK_PERIOD: 1 Hz, under 3 flashes a second. */
constexpr engine::Duration WARNING_BLINK_PERIOD =
    std::chrono::milliseconds(1000);
constexpr engine::Duration WARNING_BLINK_HIDDEN =
    std::chrono::milliseconds(250);

}  // namespace rtype::client
