#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ShipLookSlot.hpp"

namespace rtype::identifiers {

/** @brief What a slot of ShipLook holds when it wears nothing. */
constexpr std::uint8_t NO_SHIP_LOOK_ITEM = 0;

/** @brief The items of the locker, in its order: the name each one has in the
 * settings file and in the entity sheets, and its slot. */
constexpr std::size_t SHIP_LOOK_ITEM_COUNT = 10;
constexpr std::array<std::string_view, SHIP_LOOK_ITEM_COUNT>
    SHIP_LOOK_ITEM_NAMES{"top_hat",       "party_hat",  "crown",
                         "propeller_cap", "cowboy_hat", "viking_helmet",
                         "wizard_hat",    "chef_toque", "cool_shades",
                         "mustache"};
constexpr std::array<ShipLookSlot, SHIP_LOOK_ITEM_COUNT> SHIP_LOOK_ITEM_SLOTS{
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Eyes,
    ShipLookSlot::Face};

}  // namespace rtype::identifiers
