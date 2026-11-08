#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

/**
 * @brief The times of the WARNING screen the design sets, from the Arriving
 * status: it shows from 0.6 s to 3.8 s; the word gains a letter every 60 ms
 * from 0.6 s, so it holds two at 0.66 s and all seven at 0.96 s; the band
 * opens in 350 ms, to 30 rows on each side of its middle at 0.95 s, the tapes
 * sliding in meanwhile from a whole width (480 pixels) away; the band starts
 * closing 400 ms before the end, at 3.4 s, and the text leaves then; the word
 * tears 4 pixels sideways first, and no more after 500 ms.
 */
constexpr rtype::engine::Duration WARNING_SHOWS =
    std::chrono::milliseconds(600);
constexpr rtype::engine::Duration FIRST_LETTER_TIME = WARNING_SHOWS;
constexpr rtype::engine::Duration SECOND_LETTER_TIME =
    std::chrono::milliseconds(660);
constexpr rtype::engine::Duration WHOLE_WORD_TIME =
    std::chrono::milliseconds(960);
constexpr std::size_t LETTERS_OF_WARNING = 7;
constexpr rtype::engine::Duration BAND_OPEN = std::chrono::milliseconds(950);
constexpr int OPEN_BAND_HALF_HEIGHT = 30;
constexpr int WHOLE_TAPE_SHIFT = 480;
constexpr rtype::engine::Duration BAND_STARTS_CLOSING =
    std::chrono::milliseconds(3400);
constexpr rtype::engine::Duration WARNING_GONE =
    std::chrono::milliseconds(3800);
constexpr int FIRST_TEAR = 4;
constexpr rtype::engine::Duration TEAR_OVER = std::chrono::milliseconds(1100);

/** @brief The playfield the screen paints above the status band: 270 rows
 * of the interface picture, less the 24 of the band. */
constexpr int PLAYFIELD_ROWS_ABOVE_THE_BAND = 246;

/** @brief The word of the screen, its first letter, and how its fourth band
 * is clipped: rows 104 to 107 of the picture, the fourth row of the font at
 * four times its size from row 92, across the whole width. */
constexpr std::string_view WARNING_ON_SCREEN = "WARNING";
constexpr std::string_view FIRST_LETTER_ON_SCREEN = "W";
constexpr std::size_t FIRE_BAND_COUNT = 8;
constexpr std::size_t FOURTH_FIRE_BAND = 3;
constexpr sf::IntRect FOURTH_BAND_CLIP({0, 104}, {480, 4});
