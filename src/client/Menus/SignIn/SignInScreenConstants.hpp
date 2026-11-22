#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "Translation.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief What the sign-in dialog writes while it waits for a code, while a
 * player validates it, and once signed in. */
constexpr Translation SIGN_IN_TITLE{.english = "SIGN IN",
                                    .french = "CONNEXION"};
constexpr Translation ASKING_CODE_LINE{
    .english = "ASKING THE MASTER FOR A CODE...",
    .french = "DEMANDE D'UN CODE AU MAÎTRE..."};
constexpr std::string_view PAGE_MARK = "{PAGE}";
constexpr std::string_view MINUTES_MARK = "{MINUTES}";
constexpr Translation ENTER_CODE_AT{.english = "ENTER IT AT {PAGE}",
                                    .french = "À SAISIR SUR {PAGE}"};
constexpr Translation VALID_FOR{.english = "VALID FOR {MINUTES} MIN",
                                .french = "VALABLE {MINUTES} MIN"};
constexpr Translation LAST_MINUTE_TEXT{.english = "VALID FOR UNDER A MINUTE",
                                       .french = "MOINS D'UNE MINUTE"};
constexpr Translation SIGNED_IN_TITLE{.english = "SIGNED IN",
                                      .french = "CONNECTÉ"};
constexpr Translation OPENING_ACCOUNT_LINE{
    .english = "OPENING YOUR ACCOUNT...",
    .french = "OUVERTURE DE VOTRE COMPTE..."};

/** @brief What it writes when the link ends without a session. */
constexpr Translation DENIED_TITLE{.english = "CODE REFUSED",
                                   .french = "CODE REFUSÉ"};
constexpr Translation DENIED_LINE{.english = "THE SITE REFUSED THE CODE.",
                                  .french = "LE SITE A REFUSÉ LE CODE."};
constexpr Translation EXPIRED_TITLE{.english = "CODE EXPIRED",
                                    .french = "CODE EXPIRÉ"};
constexpr Translation EXPIRED_LINE{.english = "ASK FOR A NEW CODE.",
                                   .french = "DEMANDEZ UN NOUVEAU CODE."};
constexpr Translation REFUSED_TITLE{.english = "TOO MANY CODES",
                                    .french = "TROP DE CODES"};
constexpr Translation REFUSED_LINE{.english = "WAIT A WHILE, THEN ASK AGAIN.",
                                   .french = "PATIENTEZ, PUIS REDEMANDEZ."};
constexpr Translation UNREACHABLE_TITLE{.english = "NO MASTER",
                                        .french = "MAÎTRE INJOIGNABLE"};
constexpr Translation UNREACHABLE_LINE{.english = "THE MASTER DOES NOT ANSWER.",
                                       .french = "LE MAÎTRE NE RÉPOND PAS."};
constexpr Translation SESSION_REFUSED_TITLE{.english = "SIGN IN FAILED",
                                            .french = "CONNEXION ÉCHOUÉE"};
constexpr Translation SESSION_REFUSED_LINE{
    .english = "THE MASTER REFUSED THE SESSION.",
    .french = "LE MAÎTRE A REFUSÉ LA SESSION."};

/** @brief The button that asks for another code once one ended. */
constexpr Translation NEW_CODE_LABEL{.english = "NEW CODE",
                                     .french = "NOUVEAU CODE"};

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
