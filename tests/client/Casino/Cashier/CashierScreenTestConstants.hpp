#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief What the player holds, as the pictures of the cashier: 4 250
 * kopecks and 1 250 chips; what an exchange of 500 leaves, bought, then
 * cashed out. */
constexpr std::uint32_t KOPECKS_HELD = 4'250;
constexpr std::uint32_t CHIPS_HELD = 1'250;
constexpr std::uint32_t KOPECKS_AFTER_BUYING_500 = 3'750;
/** @brief The amount a cashier offers when it opens. */
constexpr std::uint32_t AMOUNT_OFFERED = 500;
constexpr std::uint32_t CHIPS_AFTER_BUYING_500 = 1'750;
constexpr std::uint32_t KOPECKS_AFTER_CASHING_OUT_500 = 4'750;
constexpr std::uint32_t CHIPS_AFTER_CASHING_OUT_500 = 750;

/** @brief The amount as the cashier opens, then one step of 100 higher and
 * lower, then a step of 500 higher; and the steps written beside it. */
constexpr std::string_view FIRST_AMOUNT = "500";
constexpr std::string_view ONE_STEP_HIGHER = "600";
constexpr std::string_view ONE_STEP_LOWER = "400";
constexpr std::string_view ONE_LARGEST_STEP_HIGHER = "1000";
constexpr std::string_view STEP_OF_100 = "STEP 100";
constexpr std::string_view STEP_OF_500 = "STEP 500";
constexpr std::string_view STEP_OF_1 = "STEP 1";

/** @brief How many steps of 500 take the amount from 500 to 5 000, 750 more
 * than the kopecks held, and what the cashier says then. */
constexpr int STEPS_TO_FIVE_THOUSAND = 9;
constexpr std::string_view SEVEN_HUNDRED_FIFTY_KOPECKS_MISSING =
    "NOT ENOUGH KOPECKS: 750 MISSING";

/** @brief What both balances hold after buying 500, as the cashier writes
 * them under AFTER. */
constexpr std::string_view KOPECKS_LEFT_AFTER_BUYING = "3 750 KOPECKS";
constexpr std::string_view CHIPS_AFTER_BUYING = "1 750 CHIPS";

/** @brief Pixels of the interface: inside the chip of 1, the first of the
 * selector, from (168, 120); inside the mark that raises the amount, from
 * (276, 93), 12 x 16. */
constexpr sf::Vector2i INSIDE_THE_CHIP_OF_ONE{175, 128};
constexpr sf::Vector2i INSIDE_THE_RAISE_MARK{281, 100};

}  // namespace rtype::client::testing
