#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ShipLookSlot.hpp"

namespace rtype::identifiers {

/** @brief What a slot of ShipLook holds when it wears nothing; in the hull
 * slot, the standard ship. */
constexpr std::uint8_t NO_SHIP_LOOK_ITEM = 0;

/**
 * @brief The items of the catalog, in its order: the name each one has in the
 * settings file and in the entity sheets, and its slot. The accessories, drawn
 * over the ship, come first; the hulls, which replace its picture, close the
 * list (see accessoriesComeFirst()).
 */
constexpr std::size_t SHIP_LOOK_ACCESSORY_COUNT = 25;
constexpr std::size_t SHIP_HULL_COUNT = 5;
constexpr std::size_t SHIP_LOOK_ITEM_COUNT =
    SHIP_LOOK_ACCESSORY_COUNT + SHIP_HULL_COUNT;
constexpr std::array<std::string_view, SHIP_LOOK_ITEM_COUNT>
    SHIP_LOOK_ITEM_NAMES{
        "top_hat",      "party_hat",     "crown",       "propeller_cap",
        "cowboy_hat",   "viking_helmet", "wizard_hat",  "chef_toque",
        "cool_shades",  "mustache",      "chapka",      "tricorn",
        "santa_hat",    "halo",          "devil_horns", "sombrero",
        "traffic_cone", "fez",           "grad_cap",    "antennae",
        "kabuto",       "mohawk",        "glasses_3d",  "monocle",
        "long_beard",   "war_shark",     "vostok",      "bydo_husk",
        "golden_ace",   "golden_bydo"};
constexpr std::array<ShipLookSlot, SHIP_LOOK_ITEM_COUNT> SHIP_LOOK_ITEM_SLOTS{
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Eyes,
    ShipLookSlot::Face, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Head, ShipLookSlot::Head,
    ShipLookSlot::Head, ShipLookSlot::Eyes, ShipLookSlot::Eyes,
    ShipLookSlot::Face, ShipLookSlot::Hull, ShipLookSlot::Hull,
    ShipLookSlot::Hull, ShipLookSlot::Hull, ShipLookSlot::Hull};

}  // namespace rtype::identifiers
