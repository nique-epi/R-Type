#pragma once

#include <array>
#include "PixelSize.hpp"

namespace rtype::client {

constexpr const char* WINDOW_TITLE = "R-Type";
constexpr unsigned int FRAMES_PER_SECOND_LIMIT = 60;

/**
 * @brief Sizes the player can give the window, from the smallest to the
 * largest.
 *
 * Every size has the proportions of the playfield, so the playfield fills the
 * window and no black bar shows. The window cannot take any other size.
 */
constexpr std::array WINDOW_SIZES{
    PixelSize{.width = 960, .height = 540},
    PixelSize{.width = 1280, .height = 720},
    PixelSize{.width = 1600, .height = 900},
    PixelSize{.width = 1920, .height = 1080},
    PixelSize{.width = 2560, .height = 1440},
};

/**
 * @brief Number of sides that share the space a window leaves free on the
 * desktop along one axis, so the window is centered.
 */
constexpr unsigned int SIDES_SHARING_FREE_SPACE = 2;

/**
 * @brief Extent of the whole window along one axis, as a fraction of itself.
 */
constexpr float WHOLE_WINDOW = 1.0F;

/**
 * @brief Share of the space the playfield leaves free that goes to each of the
 * two bars, so the playfield is centered.
 */
constexpr float BAR_SHARE_PER_SIDE = 0.5F;

/**
 * @brief Layout of the buttons that select a window size, in playfield units.
 *
 * The buttons sit in a row from the top-left corner of the playfield. Each one
 * is labelled with the size it gives, its width and height joined by the
 * separator.
 *
 * Provisional: the buttons stand in for the options menu until that one
 * exists.
 */
constexpr float WINDOW_SIZE_BUTTON_WIDTH = 230.0F;
constexpr float WINDOW_SIZE_BUTTON_HEIGHT = 60.0F;
constexpr float WINDOW_SIZE_BUTTONS_MARGIN = 24.0F;
constexpr float WINDOW_SIZE_BUTTONS_GAP = 16.0F;
constexpr unsigned int WINDOW_SIZE_LABEL_CHARACTER_SIZE = 30;
constexpr const char* WINDOW_SIZE_SEPARATOR = " x ";

/** @brief Asset id of the font of the button labels. */
constexpr const char* WINDOW_SIZE_LABEL_FONT_ID = "fonts/tuffy.ttf";

}  // namespace rtype::client
