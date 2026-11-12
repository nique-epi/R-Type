#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The first two wares of the locker: the party hat and the crown,
 * the first looks of the catalogue past the top hat of the starter kit; and
 * the cheapest hull, the war shark. Their prices are those the shop was
 * decided with. */
constexpr std::size_t PARTY_HAT_ITEM = 1;
constexpr std::size_t CROWN_ITEM = 2;
constexpr std::size_t CHEAPEST_HULL_ITEM = 25;
constexpr std::uint32_t PARTY_HAT_PRICE_PAID = 150;

/** @brief More kopecks than any ware costs, what is left of them after the
 * party hat, and a balance one kopeck under its price. */
constexpr std::uint32_t A_FORTUNE = 50000;
constexpr std::uint32_t A_FORTUNE_LESS_THE_PARTY_HAT = 49850;
constexpr std::uint32_t ONE_KOPECK_UNDER_THE_PARTY_HAT = 149;

/** @brief The status lines the shop writes. */
constexpr std::string_view PARTY_HAT_FOR_SALE_LINE =
    "PARTY HAT - ENTER: BUY FOR 150 KOPECKS";
constexpr std::string_view BOUGHT_PARTY_HAT_LINE = "BOUGHT: PARTY HAT";
constexpr std::string_view OWNED_PARTY_HAT_LINE = "PARTY HAT - ALREADY YOURS";
constexpr std::string_view ONE_KOPECK_MISSING_LINE =
    "NOT ENOUGH KOPECKS: 1 MISSING";
constexpr std::string_view ALREADY_YOURS_LINE = "ALREADY YOURS";
constexpr std::uint32_t KOPECKS_MISSING = 150;
constexpr std::string_view NOT_ENOUGH_LINE = "NOT ENOUGH KOPECKS: 150 MISSING";

/** @brief Pixels of the interface: the middle of the second cell of the
 * locker (52 + 16, 72 + 16) and of the first stall of the case (158 + 16,
 * 206 + 18). */
constexpr sf::Vector2i SECOND_CELL_MIDDLE{68, 88};
constexpr sf::Vector2i FIRST_STALL_MIDDLE_OF_THE_SHOP{174, 224};

/** @brief Pixels of the interface: the middle of the radio of the counter,
 * which the picture draws from (280, 161) over 42 x 21 pixels, and one on
 * the counter just right of it. */
constexpr sf::Vector2i RADIO_MIDDLE{301, 171};
constexpr sf::Vector2i BESIDE_THE_RADIO{326, 171};

constexpr std::string_view SHOP_SCENE_SHEET = "sprites/shop_scene.png";

}  // namespace rtype::client::testing
