#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "TextStyle.hpp"
#include "WidgetColors.hpp"

namespace rtype::client {

/** @brief The heading of the screen and the rule under it. */
constexpr sf::Vector2i HANGAR_TITLE_ANCHOR{24, 14};
constexpr TextStyle HANGAR_TITLE_STYLE{
    .font = TextFont::Regular, .color = PAGE_TEXT_COLOR, .outlined = true};
constexpr sf::IntRect HANGAR_RULE{{24, 26}, {432, 1}};

/** @brief The status line: the name of the chosen item, then what Enter does
 * with it, or that an arm is fitting it. */
constexpr sf::Vector2i HANGAR_STATUS_ANCHOR{24, 230};
constexpr TextStyle HANGAR_STATUS_STYLE{.font = TextFont::Small,
                                        .color = PAGE_TEXT_COLOR};
constexpr const char* WEAR_HINT = " - ENTER: WEAR";
constexpr const char* TAKE_OFF_HINT = " - ENTER: TAKE OFF";
constexpr const char* FITTING_HINT = " - FITTING...";

}  // namespace rtype::client
