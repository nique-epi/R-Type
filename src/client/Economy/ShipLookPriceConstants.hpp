#pragma once

#include <array>
#include <cstdint>
#include "EconomyConstants.hpp"
#include "ShipLookConstants.hpp"

namespace rtype::client {

/** @brief The price of each item, in kopecks. The starter kit is not sold. */
constexpr std::uint32_t TOP_HAT_PRICE = NOT_FOR_SALE;
constexpr std::uint32_t PARTY_HAT_PRICE = 150;
constexpr std::uint32_t CROWN_PRICE = 600;
constexpr std::uint32_t PROPELLER_CAP_PRICE = 200;
constexpr std::uint32_t COWBOY_HAT_PRICE = 250;
constexpr std::uint32_t VIKING_HELMET_PRICE = 400;
constexpr std::uint32_t WIZARD_HAT_PRICE = 350;
constexpr std::uint32_t CHEF_TOQUE_PRICE = 200;
constexpr std::uint32_t COOL_SHADES_PRICE = NOT_FOR_SALE;
constexpr std::uint32_t MUSTACHE_PRICE = NOT_FOR_SALE;
constexpr std::uint32_t CHAPKA_PRICE = 450;
constexpr std::uint32_t TRICORN_PRICE = 300;
constexpr std::uint32_t SANTA_HAT_PRICE = 250;
constexpr std::uint32_t HALO_PRICE = 600;
constexpr std::uint32_t DEVIL_HORNS_PRICE = 350;
constexpr std::uint32_t SOMBRERO_PRICE = 400;
constexpr std::uint32_t TRAFFIC_CONE_PRICE = 150;
constexpr std::uint32_t FEZ_PRICE = 200;
constexpr std::uint32_t GRAD_CAP_PRICE = 300;
constexpr std::uint32_t ANTENNAE_PRICE = 200;
constexpr std::uint32_t KABUTO_PRICE = 700;
constexpr std::uint32_t MOHAWK_PRICE = 250;
constexpr std::uint32_t GLASSES_3D_PRICE = 150;
constexpr std::uint32_t MONOCLE_PRICE = 500;
constexpr std::uint32_t LONG_BEARD_PRICE = 350;
constexpr std::uint32_t WAR_SHARK_PRICE = 2000;
constexpr std::uint32_t VOSTOK_PRICE = 2500;
constexpr std::uint32_t BYDO_HUSK_PRICE = 3000;
constexpr std::uint32_t GOLDEN_ACE_PRICE = 5000;
constexpr std::uint32_t GOLDEN_BYDO_PRICE = 10000;

/** @brief The prices, in the order of identifiers::SHIP_LOOK_ITEM_NAMES. */
constexpr std::array<std::uint32_t, identifiers::SHIP_LOOK_ITEM_COUNT>
    SHIP_LOOK_ITEM_PRICES{
        TOP_HAT_PRICE,       PARTY_HAT_PRICE,    CROWN_PRICE,
        PROPELLER_CAP_PRICE, COWBOY_HAT_PRICE,   VIKING_HELMET_PRICE,
        WIZARD_HAT_PRICE,    CHEF_TOQUE_PRICE,   COOL_SHADES_PRICE,
        MUSTACHE_PRICE,      CHAPKA_PRICE,       TRICORN_PRICE,
        SANTA_HAT_PRICE,     HALO_PRICE,         DEVIL_HORNS_PRICE,
        SOMBRERO_PRICE,      TRAFFIC_CONE_PRICE, FEZ_PRICE,
        GRAD_CAP_PRICE,      ANTENNAE_PRICE,     KABUTO_PRICE,
        MOHAWK_PRICE,        GLASSES_3D_PRICE,   MONOCLE_PRICE,
        LONG_BEARD_PRICE,    WAR_SHARK_PRICE,    VOSTOK_PRICE,
        BYDO_HUSK_PRICE,     GOLDEN_ACE_PRICE,   GOLDEN_BYDO_PRICE};

}  // namespace rtype::client
