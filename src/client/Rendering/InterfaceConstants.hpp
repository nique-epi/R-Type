#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "BackgroundConstants.hpp"

namespace rtype::client {

/**
 * @brief Asset ids of the two pixel fonts of the interface, both under the SIL
 * Open Font License: Press Start 2P, whose capitals are 7 pixels tall, for
 * bars, headings and lines laid on the sky; Silkscreen, whose capitals are 5
 * pixels tall, for small print.
 */
constexpr const char* REGULAR_FONT_ID =
    "fonts/press_start_2p/press_start_2p.ttf";
constexpr const char* SMALL_FONT_ID = "fonts/silkscreen/silkscreen.ttf";

/**
 * @brief The whole interface picture, in its own pixels: the pixel grid of the
 * background, one pixel covering BACKGROUND_PIXEL_SIZE logical units.
 */
constexpr sf::IntRect INTERFACE_AREA({0, 0}, sf::Vector2i(BACKGROUND_SIZE));

/**
 * @brief Size both fonts are drawn at, in pixels: the size they are designed
 * for, so each pixel of a letter is one pixel of the picture.
 */
constexpr unsigned int TEXT_CHARACTER_SIZE = 8;

/** @brief Height of a line of text, in pixels: as tall as the fonts. */
constexpr int TEXT_LINE_HEIGHT = static_cast<int>(TEXT_CHARACTER_SIZE);

/**
 * @brief Width of every character a text field accepts in the regular font, in
 * pixels: Press Start 2P gives each of them an advance of one em (1000 units
 * for 1000 units per em, read from the font file), drawn at
 * TEXT_CHARACTER_SIZE.
 */
constexpr int REGULAR_FONT_ADVANCE = static_cast<int>(TEXT_CHARACTER_SIZE);

/**
 * @brief Row of the baseline of each font below the top of its line, where
 * the browser placed it in the mockup for a line as tall as the font.
 */
constexpr int REGULAR_FONT_BASELINE_ROW = 8;
constexpr int SMALL_FONT_BASELINE_ROW = 7;

constexpr std::string_view INTERFACE_LOG_MODULE_NAME = "Interface";

}  // namespace rtype::client
