#pragma once

#include <array>
#include <cstdint>
#include "BackgroundColorConstants.hpp"
#include "HighContrastColorsFixtures.hpp"
#include "InterfaceColorConstants.hpp"
#include "StatusBandConstants.hpp"
#include "WidgetColors.hpp"

namespace rtype::client::testing {

/** @brief The relative luminance of WCAG 2.2: the largest channel value, the
 * limit, divisor, offset and exponent of the sRGB curve, the weights of the
 * three channels, and the flare added to both sides of a ratio. */
constexpr double CHANNEL_MAXIMUM = 255.0;
constexpr double SRGB_LINEAR_LIMIT = 0.04045;
constexpr double SRGB_LINEAR_DIVISOR = 12.92;
constexpr double SRGB_OFFSET = 0.055;
constexpr double SRGB_EXPONENT = 2.4;
constexpr double RED_WEIGHT = 0.2126;
constexpr double GREEN_WEIGHT = 0.7152;
constexpr double BLUE_WEIGHT = 0.0722;
constexpr double LUMINANCE_FLARE = 0.05;

/** @brief The ratio XAG 102 asks of every element of a high contrast mode. */
constexpr double HIGH_CONTRAST_RATIO = 7.0;

/**
 * @brief Every text and edge of the menus and of the status band, on the
 * color it is drawn on.
 */
constexpr std::array TEXTS_ON_THEIR_BACKGROUNDS{
    TextOnBackground{.text = BAR_TEXT_COLOR, .background = BAR_INSIDE_COLOR},
    TextOnBackground{.text = BAR_BORDER_COLOR, .background = BAR_INSIDE_COLOR},
    TextOnBackground{.text = SELECTED_BAR_TEXT_COLOR,
                     .background = SELECTED_BAR_INSIDE_COLOR},
    TextOnBackground{.text = PRESSED_BAR_TEXT_COLOR,
                     .background = PRESSED_BAR_INSIDE_COLOR},
    TextOnBackground{.text = DISABLED_BAR_TEXT_COLOR,
                     .background = BAR_INSIDE_COLOR},
    TextOnBackground{.text = PAGE_TEXT_COLOR, .background = SKY_COLOR},
    TextOnBackground{.text = DIALOG_HEADING_COLOR,
                     .background = PANEL_INSIDE_COLOR},
    TextOnBackground{.text = DIALOG_MESSAGE_COLOR,
                     .background = PANEL_INSIDE_COLOR},
    TextOnBackground{.text = FIELD_TEXT_COLOR,
                     .background = FIELD_INSIDE_COLOR},
    TextOnBackground{.text = SELECTED_FIELD_TEXT_COLOR,
                     .background = FIELD_INSIDE_COLOR},
    TextOnBackground{.text = FORM_DESCRIPTION_COLOR, .background = SKY_COLOR},
    TextOnBackground{.text = STATUS_TEXT_COLOR, .background = SKY_COLOR},
    TextOnBackground{.text = CURRENT_CELL_TEXT_COLOR,
                     .background = CURRENT_CELL_COLOR},
    TextOnBackground{.text = CURRENT_CELL_TEXT_COLOR,
                     .background = SELECTED_CURRENT_CELL_COLOR},
    TextOnBackground{.text = BOXED_VALUE_COLOR,
                     .background = FIELD_INSIDE_COLOR},
    TextOnBackground{.text = LIST_HIGHLIGHT_TEXT_COLOR,
                     .background = LIST_HIGHLIGHT_COLOR},
    TextOnBackground{.text = UNAVAILABLE_CHOICE_COLOR,
                     .background = LIST_INSIDE_COLOR},
    TextOnBackground{.text = KEY_CAP_TEXT_COLOR,
                     .background = KEY_CAP_FACE_COLOR},
    TextOnBackground{.text = TAB_TEXT_COLOR, .background = TAB_INSIDE_COLOR},
    TextOnBackground{.text = TAB_BORDER_COLOR, .background = TAB_INSIDE_COLOR},
    TextOnBackground{.text = STATUS_BAND_BORDER_COLOR,
                     .background = STATUS_BAND_INSIDE_COLOR},
    TextOnBackground{.text = GONE_TEXT_COLOR,
                     .background = STATUS_BAND_INSIDE_COLOR},
    TextOnBackground{.text = LEADING_ZERO_COLOR,
                     .background = STATUS_BAND_INSIDE_COLOR},
    TextOnBackground{.text = STATUS_LABEL_COLOR,
                     .background = STATUS_BAND_INSIDE_COLOR},
    TextOnBackground{.text = WARNING_TEXT_COLOR,
                     .background = STATUS_BAND_INSIDE_COLOR},
};

/** @brief A color HIGH CONTRAST leaves as it is, and a translucent one it
 * replaces while keeping its opacity. */
constexpr sf::Color UNTOUCHED_COLOR = STAR_GOLD;
constexpr sf::Color TRANSLUCENT_SKY = DIM_LAYER_COLOR;

}  // namespace rtype::client::testing
