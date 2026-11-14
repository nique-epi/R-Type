#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The middles of two cards on the interface, as the design lays
 * them: the one in the middle from (162, 48), 156 x 164, the one at the
 * right from (338, 70), 104 x 120. */
constexpr sf::Vector2i CHOSEN_CARD_MIDDLE{240, 130};
constexpr sf::Vector2i RIGHT_CARD_MIDDLE{390, 130};

/** @brief The house of the hangar, 9 pixels at top 47 of the sheet of the
 * map, and the largest whole factor at which it fits the box of 64 pixels a
 * sprite of a card fills: 7, 63 pixels. */
constexpr std::string_view MAP_SHEET = "sprites/sector_map.png";
constexpr sf::IntRect HANGAR_HOUSE_SOURCE{{0, 47}, {9, 9}};
constexpr int HOUSE_FILLING_THE_BOX = 7;

/** @brief The name of the card in the middle is written twice the size of a
 * line. */
constexpr int TWICE_A_LINE = 2;

/** @brief What the description of SERVER says. */
constexpr std::string_view SERVER_DESCRIPTION_LINE =
    "JOIN A SECTOR AND FLY WITH UP TO 4 PILOTS";

/** @brief A balance of the player, and how it is written: thousands apart. */
constexpr std::uint32_t BALANCE = 12450;
constexpr std::string_view BALANCE_TEXT = "12 450";

/** @brief A balance spent in the shop, and how it is written. */
constexpr std::uint32_t SPENT_BALANCE = 450;
constexpr std::string_view SPENT_BALANCE_TEXT = "450";

}  // namespace rtype::client::testing
