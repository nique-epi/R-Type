#pragma once

#include <array>
#include <cstddef>
#include "DisplayMode.hpp"
#include "PixelPosition.hpp"
#include "PixelSize.hpp"

namespace rtype::client {

constexpr const char* WINDOW_TITLE = "R-Type";
constexpr unsigned int FRAMES_PER_SECOND_LIMIT = 60;

/** @brief Each size a windowed window can take, named after its width and its
 * height in pixels. */
constexpr PixelSize WINDOW_SIZE_960_BY_540{.width = 960, .height = 540};
constexpr PixelSize WINDOW_SIZE_1280_BY_720{.width = 1280, .height = 720};
constexpr PixelSize WINDOW_SIZE_1600_BY_900{.width = 1600, .height = 900};
constexpr PixelSize WINDOW_SIZE_1920_BY_1080{.width = 1920, .height = 1080};
constexpr PixelSize WINDOW_SIZE_2560_BY_1440{.width = 2560, .height = 1440};

/**
 * @brief Sizes the player can give the window, from the smallest to the
 * largest.
 *
 * Every size has the proportions of the playfield, so the playfield fills the
 * window and no black bar shows. A windowed window cannot take any other size.
 */
constexpr std::array WINDOW_SIZES{
    WINDOW_SIZE_960_BY_540,   WINDOW_SIZE_1280_BY_720,  WINDOW_SIZE_1600_BY_900,
    WINDOW_SIZE_1920_BY_1080, WINDOW_SIZE_2560_BY_1440,
};

/** @brief Every display mode, in the order the options list them. */
constexpr std::array DISPLAY_MODES{DisplayMode::Windowed,
                                   DisplayMode::Borderless};
constexpr std::size_t DISPLAY_MODE_COUNT = DISPLAY_MODES.size();

/** @brief Where a window that covers the whole desktop starts: its top-left
 * corner. */
constexpr PixelPosition DESKTOP_CORNER{.x = 0, .y = 0};

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

/** @brief What joins the width and the height of a window size in a text. */
constexpr const char* WINDOW_SIZE_TEXT_SEPARATOR = " X ";

}  // namespace rtype::client
