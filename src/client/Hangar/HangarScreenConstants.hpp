#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "TextStyle.hpp"
#include "Translation.hpp"
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
constexpr Translation WEAR_HINT{.english = " - ENTER: WEAR",
                                .french = " - ENTRÉE : PORTER"};
constexpr Translation TAKE_OFF_HINT{.english = " - ENTER: TAKE OFF",
                                    .french = " - ENTRÉE : RETIRER"};

/** @brief The status line of an item the player does not hold yet: a look
 * item is won in a mission or bought, a hull is only bought. */
constexpr Translation LOCKED_STATUS{
    .english = "LOCKED: WIN IT IN A MISSION OR BUY IT",
    .french = "VERROUILLÉ : À GAGNER EN MISSION OU À ACHETER"};
constexpr Translation LOCKED_HULL_STATUS{
    .english = "LOCKED: BUY IT AT THE SHOP",
    .french = "VERROUILLÉ : À ACHETER AU MAGASIN"};
constexpr Translation FITTING_HINT{.english = " - FITTING...",
                                   .french = " - MONTAGE..."};

}  // namespace rtype::client
