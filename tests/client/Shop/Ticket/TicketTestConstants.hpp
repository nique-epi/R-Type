#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/** @brief The ticket lies at (154, 27) of the interface; its ware area at
 * (50, 42) of it, 72 by 40, so at (204, 69) of the interface, and its middle
 * column is 86 of it, 240 of the interface. */
constexpr sf::IntRect WARE_AREA_ON_SCREEN{{204, 69}, {72, 40}};
constexpr int TICKET_MIDDLE_ON_SCREEN = 240;

/** @brief The chapka, 14 by 9, is drawn three times as large, the most an
 * icon is enlarged: 42 by 27, centred in the ware area, at (219, 75); its
 * middle, 219 + 21, is column 240. Its name is typed at row 88 of the
 * ticket, 115 of the interface. */
constexpr std::size_t CHAPKA_ITEM = 10;
constexpr std::string_view LOOKS_SHEET = "sprites/looks.png";
constexpr sf::Vector2i CHAPKA_TOP_LEFT{219, 75};
constexpr int CHAPKA_SCALE = 3;
constexpr std::string_view CHAPKA_NAME = "CHAPKA";
constexpr sf::Vector2i CHAPKA_NAME_ANCHOR{240, 115};
constexpr std::array<std::string_view, 3> CHAPKA_DESCRIPTION{
    "GENUINE MOSCOW FUR.", "WARM DOWN TO -40.", "COMRADE-APPROVED."};

/** @brief The ink of the typed text, dark on the paper. */
constexpr sf::Color TYPED_INK(43, 38, 64);

/** @brief The war shark, a hull, 32 by 16, is drawn twice as large: 64 by
 * 32, centred in the ware area, at (208, 73). */
constexpr std::size_t WAR_SHARK_ITEM = 25;
constexpr std::string_view HULLS_SHEET = "sprites/hulls.png";
constexpr sf::Vector2i WAR_SHARK_TOP_LEFT{208, 73};
constexpr int WAR_SHARK_SCALE = 2;

/** @brief The PAID stamp lies at (0, 236) of the ticket sheet, 88 by 34, and
 * is laid at (42, 153) of the ticket, (196, 180) of the interface. */
constexpr std::string_view TICKET_SHEET_ON_TICKET = "sprites/shop_ticket.png";
constexpr sf::IntRect PAID_STAMP_SOURCE{{0, 236}, {88, 34}};
constexpr sf::Vector2i PAID_STAMP_ON_SCREEN{196, 180};
constexpr std::string_view BUY_WORD = "BUY";

/** @brief Nineteen letters of the regular font, eight pixels each: the text
 * of the ticket stands 10 pixels in from each side of its 172. */
constexpr int TICKET_TEXT_WIDTH_ON_PAPER = 152;

}  // namespace rtype::client::testing
