#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ShipLook.hpp"

namespace rtype::client::testing {

/** @brief Items by their place in the locker: the top hat, the party hat,
 * the shades and the mustache; what a slot holds is that place plus one. */
constexpr std::size_t TOP_HAT = 0;
constexpr std::size_t PARTY_HAT = 1;
constexpr std::size_t COOL_SHADES = 8;
constexpr std::size_t MUSTACHE = 9;
constexpr std::uint8_t WEARS_TOP_HAT = 1;
constexpr std::uint8_t WEARS_PARTY_HAT = 2;
constexpr std::uint8_t WEARS_SHADES = 9;
constexpr std::uint8_t WEARS_NOTHING = 0;

/** @brief A ship wearing an item in every slot. */
constexpr rtype::identifiers::ShipLook FULLY_DRESSED{
    .head = 1, .eyes = 9, .face = 10};

/** @brief A ship drawn at (100, 100), twice its size: the brim of the top hat
 * sits on its top fin, 9 pixels right and 7 up of its corner, so its drawing
 * starts at (100 + 9 x 2, 100 - 7 x 2). */
constexpr sf::Vector2i SHIP_CORNER(100, 100);
constexpr int TWICE = 2;
constexpr sf::Vector2i TOP_HAT_CORNER(118, 86);
constexpr std::string_view LOOK_SHEET = "sprites/looks.png";

}  // namespace rtype::client::testing
