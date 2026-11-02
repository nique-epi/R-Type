#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "HudSpriteConstants.hpp"
#include "InterfaceConstants.hpp"
#include "StatusBandConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How far the notices of a game keep from the edges of the
 * playfield. */
constexpr int NOTICE_EDGE = 4;

/**
 * @brief The feed of departures, from DEPARTURE_FEED_TOP_LEFT: a line per
 * pilot gone, the newest at the top, DEPARTURE_FEED_LINES at most, each
 * DEPARTURE_LINE_HEIGHT rows tall and DEPARTURE_LINE_GAP under the previous
 * one, kept DEPARTURE_LINE_TIME then gone at once. A line holds the icon of
 * the departure in a column as wide as the door: the door for a pilot who
 * left, the plug of the status band (STATUS_ICON_SIZE) at the right of the
 * column for a lost signal; then, DEPARTURE_PIECE_GAP after the column, the
 * name of the pilot cut to DEPARTURE_NAME_LETTERS letters, in the light shade
 * of its color; then, DEPARTURE_WORD_GAP after it, LEFT THE GAME or SIGNAL
 * LOST in small print. The name and the words are written
 * DEPARTURE_TEXT_DROP under the top of the line, on the same row, so their
 * capitals end on the same baseline beside the middle of the door, and the
 * plug starts on that row too. Every text is outlined in ink.
 */
constexpr sf::Vector2i DEPARTURE_FEED_TOP_LEFT(NOTICE_EDGE, NOTICE_EDGE + 1);
constexpr int DEPARTURE_LINE_HEIGHT = LEFT_GAME_FRAME.height;
constexpr int DEPARTURE_LINE_GAP = 2;
constexpr std::size_t DEPARTURE_FEED_LINES = 3;
constexpr engine::Duration DEPARTURE_LINE_TIME = std::chrono::seconds(5);
constexpr int DEPARTURE_ICON_COLUMN = LEFT_GAME_FRAME.width;
constexpr int DEPARTURE_PIECE_GAP = 2;
constexpr int DEPARTURE_TEXT_DROP = 2;
constexpr sf::Vector2i DEPARTURE_PLUG_OFFSET(DEPARTURE_ICON_COLUMN -
                                                 STATUS_ICON_SIZE.x,
                                             DEPARTURE_TEXT_DROP);
constexpr std::size_t DEPARTURE_NAME_LETTERS = 10;
constexpr int DEPARTURE_WORD_GAP = 6;
constexpr const char* LEFT_THE_GAME_TEXT = "LEFT THE GAME";
constexpr sf::Color DEPARTURE_WORD_COLOR = GONE_TEXT_COLOR;

/**
 * @brief The warning of a silent server, once no message of the server came
 * for SILENCE_WARNING_DELAY in a game: the bars of a weak signal NOTICE_EDGE
 * from the top-right corner of the playfield, and WAITING FOR THE SERVER in
 * small print, outlined in ink, its right edge SILENCE_WARNING_GAP left of the
 * bars, SILENCE_WARNING_DROP lower than their top.
 */
constexpr engine::Duration SILENCE_WARNING_DELAY = std::chrono::seconds(1);
constexpr std::string_view SILENCE_WARNING_TEXT = "WAITING FOR THE SERVER";
constexpr sf::Vector2i WEAK_SIGNAL_TOP_LEFT(
    INTERFACE_AREA.size.x - NOTICE_EDGE - WEAK_SIGNAL_FRAME.width, NOTICE_EDGE);
constexpr int SILENCE_WARNING_GAP = 4;
constexpr int SILENCE_WARNING_DROP = 2;
constexpr sf::Color SILENCE_WARNING_COLOR = GONE_TEXT_COLOR;

}  // namespace rtype::client
