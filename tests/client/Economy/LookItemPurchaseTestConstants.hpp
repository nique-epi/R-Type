#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "NamedPrice.hpp"

namespace rtype::client::testing {

/** @brief The prices the shop was decided with, by the name of each item. */
constexpr std::array CATALOGUE_PRICES{
    NamedPrice{.name = "top_hat", .price = 0},
    NamedPrice{.name = "party_hat", .price = 150},
    NamedPrice{.name = "crown", .price = 600},
    NamedPrice{.name = "propeller_cap", .price = 200},
    NamedPrice{.name = "cowboy_hat", .price = 250},
    NamedPrice{.name = "viking_helmet", .price = 400},
    NamedPrice{.name = "wizard_hat", .price = 350},
    NamedPrice{.name = "chef_toque", .price = 200},
    NamedPrice{.name = "cool_shades", .price = 0},
    NamedPrice{.name = "mustache", .price = 0},
    NamedPrice{.name = "chapka", .price = 450},
    NamedPrice{.name = "tricorn", .price = 300},
    NamedPrice{.name = "santa_hat", .price = 250},
    NamedPrice{.name = "halo", .price = 600},
    NamedPrice{.name = "devil_horns", .price = 350},
    NamedPrice{.name = "sombrero", .price = 400},
    NamedPrice{.name = "traffic_cone", .price = 150},
    NamedPrice{.name = "fez", .price = 200},
    NamedPrice{.name = "grad_cap", .price = 300},
    NamedPrice{.name = "antennae", .price = 200},
    NamedPrice{.name = "kabuto", .price = 700},
    NamedPrice{.name = "mohawk", .price = 250},
    NamedPrice{.name = "glasses_3d", .price = 150},
    NamedPrice{.name = "monocle", .price = 500},
    NamedPrice{.name = "long_beard", .price = 350},
    NamedPrice{.name = "war_shark", .price = 2000},
    NamedPrice{.name = "vostok", .price = 2500},
    NamedPrice{.name = "bydo_husk", .price = 3000},
    NamedPrice{.name = "golden_ace", .price = 5000},
    NamedPrice{.name = "golden_bydo", .price = 10000},
};

constexpr std::string_view CROWN_NAME = "crown";
constexpr std::string_view TOP_HAT_NAME = "top_hat";
constexpr std::uint32_t CROWN_PRICE_PAID = 600;
constexpr std::uint32_t RICH_BALANCE = 1000;
constexpr std::uint32_t BALANCE_AFTER_THE_CROWN = 400;
constexpr std::uint32_t ONE_KOPECK_SHORT_OF_THE_CROWN = 599;

}  // namespace rtype::client::testing
