#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "ScreenId.hpp"

namespace rtype::client::testing {

/** @brief The second the tests read the clock at, and the expiry of a code
 * nine and a half minutes later, then 30 seconds later. */
constexpr std::chrono::sys_seconds SIGN_IN_NOW{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds NINE_AND_A_HALF_MINUTES_LATER =
    SIGN_IN_NOW + std::chrono::seconds{570};
constexpr std::chrono::sys_seconds HALF_A_MINUTE_LATER =
    SIGN_IN_NOW + std::chrono::seconds{30};

/** @brief The code a link shows, its pages, the token it gives, and what
 * the dialog writes of them: nine whole minutes left, or under a minute. */
constexpr std::string_view SHOWN_CODE = "KQFB-WZPM";
constexpr std::string_view SHOWN_PAGE = "https://rtype.app/link";
constexpr std::string_view SHOWN_PAGE_WITH_CODE =
    "https://rtype.app/link?code=KQFB-WZPM";
constexpr std::string_view SIGN_IN_TOKEN =
    "rts_fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
constexpr std::string_view PAGE_LINE = "ENTER IT AT RTYPE.APP/LINK";
constexpr std::string_view NINE_MINUTES_VALIDITY = "VALID FOR 9 MIN";
constexpr std::string_view LAST_MINUTE_VALIDITY = "VALID FOR UNDER A MINUTE";
constexpr std::string_view PAGE_AS_SHOWN = "RTYPE.APP/LINK";

/** @brief The account the token opens, and the screen that asked to sign
 * in. */
constexpr std::uint64_t SIGNED_IN_ACCOUNT = 7;
constexpr ScreenId ASKING_SCREEN{"casino of the tests"};

/** @brief The dialog of a waiting code, as its picture draws it: a panel of
 * 272 x 102 at (104, 84), (480 - 272) / 2 and (270 - 102) / 2. The code is
 * anchored by the middle of its top, x 480 / 2 = 240, 12 down; the page line
 * 16 in and 36 down, the validity 48 down; BACK, alone, has its bar 72 down
 * at x 104 + (272 - 64) / 2 = 208, its label 12 in and 6 down. */
constexpr sf::Vector2i WAITING_CODE_ANCHOR(240, 96);
constexpr sf::Vector2i PAGE_LINE_ANCHOR(120, 120);
constexpr sf::Vector2i VALIDITY_ANCHOR(120, 132);
constexpr sf::Vector2i BACK_UNDER_THE_CODE(220, 162);

/** @brief The code in yellow, twice the size of a line; how long it stays
 * valid in blue. */
constexpr sf::Color CODE_YELLOW = STAR_GOLD;
constexpr int CODE_TWICE_A_LINE = 2;
constexpr sf::Color VALIDITY_BLUE = HUD_CYAN;

/** @brief The title of an expired code, where a dialog of 272 x 96 writes
 * its heading: 16 in and 12 down from its panel at (104, 87). */
constexpr sf::Vector2i EXPIRED_TITLE_ANCHOR(120, 99);

}  // namespace rtype::client::testing
