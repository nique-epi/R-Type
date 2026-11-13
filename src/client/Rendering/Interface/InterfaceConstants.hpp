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

/** @brief Length of each of the two strokes of a corner paintBoxCorners()
 * paints, in pixels. */
constexpr int BOX_CORNER_LENGTH = 8;

/**
 * @brief Half pixels of the picture one pixel of a font covers when the text
 * is drawn as the fonts are designed, and how many half pixels make a pixel.
 * A text size is a count of half pixels, so a font drawn half again as large
 * still lands on whole pixels of a canvas of CRISP_TEXT_DENSITY.
 */
constexpr int NORMAL_TEXT_HALF_PIXELS = 2;
constexpr int HALF_PIXELS_PER_PIXEL = 2;

/** @brief How far the plate HIGH CONTRAST lays under a line reaches past
 * its letters on each side, in pixels, and how many sides of a line it grows
 * on along each axis. */
constexpr float PLATE_MARGIN = 1.0F;
constexpr float SIDES_OF_A_PLATE = 2.0F;

/** @brief Scale of an image drawImage() copies pixel for pixel. */
constexpr int NATIVE_SCALE = 1;

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

constexpr std::string_view INTERFACE_LOGGER_NAME = "Interface";

/** @brief How a player is named on screen: P and its number, on every bay of
 * the waiting room, in the status band where no nickname is shown, and before
 * the figure of a gain only that player made. */
constexpr const char* PLAYER_NUMBER_PREFIX = "P";

}  // namespace rtype::client
