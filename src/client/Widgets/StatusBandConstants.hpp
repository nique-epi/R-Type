#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "PlayfieldConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief Rows of the interface picture the status band covers: the
 * STATUS_BAND_HEIGHT of the playfield, in pixels of the picture. */
constexpr int STATUS_BAND_ROWS =
    static_cast<int>(game::STATUS_BAND_HEIGHT / BACKGROUND_PIXEL_SIZE);

static_assert(static_cast<float>(STATUS_BAND_ROWS) * BACKGROUND_PIXEL_SIZE ==
                  game::STATUS_BAND_HEIGHT,
              "the status band covers whole pixels of the picture");

/** @brief The status band: the whole width of the picture, its bottom
 * STATUS_BAND_ROWS rows. */
constexpr sf::IntRect STATUS_BAND_AREA(
    {0, INTERFACE_AREA.size.y - STATUS_BAND_ROWS},
    {INTERFACE_AREA.size.x, STATUS_BAND_ROWS});

/**
 * @brief The bays of the band, one per player color, left to right from
 * player 1: STATUS_BAY_WIDTH wide and STATUS_BAY_GAP apart, the first
 * STATUS_BAY_INSET from the left edge, as on the picture the team chose
 * (proposal B of the HUD design system, nickname in title). A rule of
 * STATUS_RULE_HEIGHT separates two bays, STATUS_RULE_TOP below the top of the
 * band.
 */
constexpr int STATUS_BAY_INSET = 1;
constexpr int STATUS_BAY_GAP = 2;
constexpr int STATUS_BAY_WIDTH =
    (INTERFACE_AREA.size.x - (SIDES_PER_AXIS * STATUS_BAY_INSET) -
     ((PLAYER_COLOR_COUNT - 1) * STATUS_BAY_GAP)) /
    PLAYER_COLOR_COUNT;
constexpr int STATUS_RULE_TOP = 4;
constexpr int STATUS_RULE_HEIGHT = 16;

/**
 * @brief Where the parts of a bay lie, from the top-left corner of the bay.
 *
 * First row: the chip of the pilot, STATUS_CHIP_SIZE, its marker at
 * STATUS_MARKER_OFFSET from the chip; the name of the pilot from
 * STATUS_NAME_TOP_LEFT; its word ending at STATUS_WORD_END, SMALL_WORD_DROP
 * lower in small print, STATUS_WORD_GAP after the name at least. Second row:
 * the lives from STATUS_LIVES_TOP_LEFT, STATUS_LIFE_STEP apart, or one life
 * and its count at STATUS_LIVES_COUNT_OFFSET, or the plug of a lost signal,
 * STATUS_ICON_SIZE; then the score, whose last digit ends at
 * STATUS_SCORE_END. The frame of the local player, STATUS_LOCAL_FRAME, stands
 * STATUS_LOCAL_FRAME_INSET in from each side of the bay, as tall as the
 * band.
 */
constexpr sf::Vector2i STATUS_CHIP_TOP_LEFT(5, 3);
constexpr sf::Vector2i STATUS_CHIP_SIZE(11, 10);
constexpr sf::Vector2i STATUS_MARKER_OFFSET(2, 1);
constexpr sf::Vector2i STATUS_NAME_TOP_LEFT(19, 4);
constexpr sf::Vector2i STATUS_WORD_END(112, 4);
constexpr int SMALL_WORD_DROP = 1;
constexpr int STATUS_WORD_GAP = 4;
constexpr sf::Vector2i STATUS_LIVES_TOP_LEFT(5, 14);
constexpr int STATUS_LIFE_STEP = 17;
constexpr sf::Vector2i STATUS_LIVES_COUNT_OFFSET(18, 0);
constexpr sf::Vector2i STATUS_ICON_SIZE(8, 8);
constexpr sf::Vector2i STATUS_SCORE_END(112, 14);
constexpr int STATUS_LOCAL_FRAME_INSET = 1;
constexpr sf::IntRect STATUS_LOCAL_FRAME(
    {STATUS_LOCAL_FRAME_INSET, 0},
    {STATUS_BAY_WIDTH - (SIDES_PER_AXIS * STATUS_LOCAL_FRAME_INSET),
     STATUS_BAND_ROWS});

/** @brief Digits of a score, leading zeros included, so a growing score never
 * moves. */
constexpr int STATUS_SCORE_DIGITS = 7;

/** @brief Base of the digits of a score. */
constexpr std::uint32_t DECIMAL_BASE = 10;

/** @returns The largest number written on @p digits decimal digits. */
constexpr std::uint32_t largestNumberOn(int digits) {
  std::uint32_t largest = 0;
  for (int digit = 0; digit < digits; ++digit) {
    largest = (largest * DECIMAL_BASE) + (DECIMAL_BASE - 1);
  }
  return largest;
}

/** @brief The largest score a bay shows, 9999999: a higher score stops there,
 * as the counter of an arcade cabinet does, so its digits never run over the
 * lives. */
constexpr std::uint32_t LARGEST_SHOWN_SCORE =
    largestNumberOn(STATUS_SCORE_DIGITS);
constexpr char LEADING_ZERO = '0';

/** @brief Slots of lives of a bay: a ship for each life left, a lost life
 * for each of the others; past it, one ship and the count. */
constexpr int STATUS_LIFE_ICON_LIMIT = 3;

/** @brief Lives of a pilot on its last life, which the band warns of. */
constexpr int LAST_LIFE_COUNT = 1;

/** @brief Words of the band. */
constexpr const char* LIVES_COUNT_PREFIX = "x";
constexpr const char* OUT_TEXT = "OUT";
constexpr const char* LAST_LIFE_TEXT = "LAST LIFE";
constexpr const char* RESPAWN_TEXT = "RESPAWN";
constexpr const char* LEFT_GAME_TEXT = "LEFT";
constexpr const char* SIGNAL_LOST_TEXT = "SIGNAL LOST";

/** @brief Colors of the band. A pilot gone from the game, out of lives, left
 * or dropped, has its chip, its name and its score dimmed. */
constexpr sf::Color STATUS_BAND_BORDER_COLOR = DEEP_BLUE;
constexpr sf::Color STATUS_BAND_INSIDE_COLOR = SKY_COLOR;
constexpr sf::Color STATUS_RULE_COLOR = DEEP_BLUE;
constexpr sf::Color GONE_CHIP_COLOR = DEEP_BLUE;
constexpr sf::Color GONE_TEXT_COLOR = PALE_BLUE;
constexpr sf::Color SCORE_COLOR = STAR_WHITE;
constexpr sf::Color LEADING_ZERO_COLOR = BLUE;
constexpr sf::Color STATUS_LABEL_COLOR = HUD_CYAN;
constexpr sf::Color WARNING_TEXT_COLOR = WARNING_ORANGE;
constexpr sf::Color LOCAL_BAY_FRAME_COLOR = STAR_WHITE;

}  // namespace rtype::client
