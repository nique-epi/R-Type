#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "InterfaceConstants.hpp"
#include "SquadronConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief The account chips of the title: SIGN IN, with SIGN_IN_KEY, in the
 * top-right corner for a guest, where the balance of a player stands; SIGN
 * OUT, with SIGN_OUT_KEY, in the bottom-right corner for a player signed in,
 * TITLE_SIGN_OUT_FOOTER_GAP over the footer, its nickname TITLE_NICKNAME_GAP
 * over the chip, in TITLE_NICKNAME_STYLE.
 *
 * SIGN OUT ends on row 236 and the nickname starts on row 210, two rows under
 * the squadron box, which ends on row 208.
 */
constexpr sf::Keyboard::Key SIGN_IN_KEY = sf::Keyboard::Key::I;
constexpr const char* SIGN_IN_KEY_NAME = "I";
constexpr sf::Keyboard::Key SIGN_OUT_KEY = sf::Keyboard::Key::O;
constexpr const char* SIGN_OUT_KEY_NAME = "O";
constexpr const char* SIGN_OUT_LABEL = "SIGN OUT";
constexpr int TITLE_SIGN_OUT_FOOTER_GAP = 5;
constexpr int TITLE_NICKNAME_GAP = 2;
constexpr int TITLE_SIGN_OUT_TOP =
    FOOTER_TOP - TITLE_SIGN_OUT_FOOTER_GAP - CHIP_HEIGHT;
constexpr int TITLE_NICKNAME_TOP =
    TITLE_SIGN_OUT_TOP - TITLE_NICKNAME_GAP - TEXT_LINE_HEIGHT;
static_assert(TITLE_NICKNAME_TOP >= ILLUSTRATION_TOP + SQUADRON_BOX_SIZE.y,
              "the nickname stays under the squadron box");
constexpr TextStyle TITLE_NICKNAME_STYLE{.font = TextFont::Small,
                                         .color = BAR_TEXT_COLOR,
                                         .alignment = TextAlignment::Right};

}  // namespace rtype::client
