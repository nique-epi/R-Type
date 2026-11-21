#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief What the sign-in dialog writes while it waits for a code, while a
 * player validates it, and once signed in. */
constexpr std::string_view SIGN_IN_TITLE = "SIGN IN";
constexpr std::string_view ASKING_CODE_LINE = "ASKING THE MASTER FOR A CODE...";
constexpr std::string_view ENTER_CODE_AT_START = "ENTER IT AT ";
constexpr std::string_view VALID_FOR_START = "VALID FOR ";
constexpr std::string_view MINUTES_END = " MIN";
constexpr std::string_view LAST_MINUTE_TEXT = "VALID FOR UNDER A MINUTE";
constexpr std::string_view SIGNED_IN_TITLE = "SIGNED IN";
constexpr std::string_view OPENING_ACCOUNT_LINE = "OPENING YOUR ACCOUNT...";

/** @brief What it writes when the link ends without a session. */
constexpr std::string_view DENIED_TITLE = "CODE REFUSED";
constexpr std::string_view DENIED_LINE = "THE SITE REFUSED THE CODE.";
constexpr std::string_view EXPIRED_TITLE = "CODE EXPIRED";
constexpr std::string_view EXPIRED_LINE = "ASK FOR A NEW CODE.";
constexpr std::string_view REFUSED_TITLE = "TOO MANY CODES";
constexpr std::string_view REFUSED_LINE = "WAIT A WHILE, THEN ASK AGAIN.";
constexpr std::string_view UNREACHABLE_TITLE = "NO MASTER";
constexpr std::string_view UNREACHABLE_LINE = "THE MASTER DOES NOT ANSWER.";
constexpr std::string_view SESSION_REFUSED_TITLE = "SIGN IN FAILED";
constexpr std::string_view SESSION_REFUSED_LINE =
    "THE MASTER REFUSED THE SESSION.";

/** @brief The button that asks for another code once one ended. */
constexpr const char* NEW_CODE_LABEL = "NEW CODE";

/** @brief The schemes left out of the address the dialog writes, so the
 * player reads `RTYPE.APP/LINK`. */
constexpr std::string_view SECURE_SCHEME = "https://";
constexpr std::string_view PLAIN_SCHEME = "http://";

/** @brief The unit the dialog counts the life of a code in: a minute, so its
 * menu is built again once a minute at most, and keeps the selection of the
 * player in between. */
constexpr std::chrono::minutes SIGN_IN_COUNT_UNIT{1};

/**
 * @brief How the dialog shows a code while it waits, in the area it reserves
 * from where its heading goes: the code twice the size of a line, in gold,
 * centered; SIGN_IN_SPACE_UNDER_CODE under it, the page to enter it at, in
 * white, against the left side; SIGN_IN_SPACE_BETWEEN_LINES under that, how
 * long it stays valid, in cyan. The area is as wide as the panel less the
 * margin of its heading on each side, 272 - 2 x 16 = 240 pixels, and 16 + 8 +
 * 8 + 4 + 8 = 44 tall; its longest line, ENTER IT AT RTYPE.APP/LINK, is 26 x
 * 8 = 208 pixels wide.
 */
constexpr int SIGN_IN_CODE_SCALE = 2;
constexpr int SIGN_IN_SPACE_UNDER_CODE = 8;
constexpr int SIGN_IN_SPACE_BETWEEN_LINES = 4;
constexpr sf::Vector2i SIGN_IN_PAGE_LINE_OFFSET(
    0, (TEXT_LINE_HEIGHT * SIGN_IN_CODE_SCALE) + SIGN_IN_SPACE_UNDER_CODE);
constexpr sf::Vector2i SIGN_IN_VALIDITY_OFFSET(0,
                                               SIGN_IN_PAGE_LINE_OFFSET.y +
                                                   TEXT_LINE_HEIGHT +
                                                   SIGN_IN_SPACE_BETWEEN_LINES);
constexpr sf::Vector2i SIGN_IN_CODE_AREA_SIZE(
    DIALOG_SIZE.x - (SIDES_PER_AXIS * DIALOG_HEADING_OFFSET.x),
    SIGN_IN_VALIDITY_OFFSET.y + TEXT_LINE_HEIGHT);
constexpr TextStyle SIGN_IN_CODE_STYLE{.font = TextFont::Regular,
                                       .color = STAR_GOLD,
                                       .alignment = TextAlignment::Center,
                                       .scale = SIGN_IN_CODE_SCALE};
constexpr TextStyle SIGN_IN_PAGE_LINE_STYLE{.font = TextFont::Regular,
                                            .color = STAR_WHITE};
constexpr TextStyle SIGN_IN_VALIDITY_STYLE{.font = TextFont::Regular,
                                           .color = HUD_CYAN};

}  // namespace rtype::client
