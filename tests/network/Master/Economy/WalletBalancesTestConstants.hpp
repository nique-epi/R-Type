#pragma once

#include <cstdint>
#include "Wallet.hpp"

/** @brief A wallet ten chips short of the largest balance, 999999. */
constexpr rtype::network::Wallet TEN_CHIPS_SHORT_OF_THE_MAXIMUM{
    .kopecks = 0, .chips = 999'989};
constexpr std::uint32_t TEN_CHIPS = 10;
constexpr std::uint32_t ELEVEN_CHIPS = 11;
constexpr std::uint64_t A_HUNDRED_CHIPS = 100;
constexpr std::uint32_t LARGEST_CHIPS = 999'999;
constexpr std::uint32_t LARGEST_KOPECKS = 999'999;

/** @brief A wallet of the cashier pictures, 4 250 kopecks and 1 250 chips,
 * and an exchange of 500, one for one: 3 750 kopecks and 1 750 chips once
 * bought, 4 750 kopecks and 750 chips once cashed out. */
constexpr std::uint32_t SOME_KOPECKS = 4'250;
constexpr std::uint32_t SOME_CHIPS = 1'250;
constexpr rtype::network::Wallet A_WALLET{.kopecks = SOME_KOPECKS,
                                          .chips = SOME_CHIPS};
constexpr std::uint32_t AN_AMOUNT = 500;
constexpr rtype::network::Wallet A_WALLET_AFTER_BUYING{.kopecks = 3'750,
                                                       .chips = 1'750};
constexpr rtype::network::Wallet A_WALLET_AFTER_CASHING_OUT{.kopecks = 4'750,
                                                            .chips = 750};

/** @brief Wallets holding one balance only, and the same wallets once that
 * whole balance changed currency. */
constexpr rtype::network::Wallet KOPECKS_ONLY{.kopecks = SOME_KOPECKS,
                                              .chips = 0};
constexpr rtype::network::Wallet ALL_KOPECKS_BOUGHT{.kopecks = 0,
                                                    .chips = SOME_KOPECKS};
constexpr rtype::network::Wallet CHIPS_ONLY{.kopecks = 0, .chips = SOME_CHIPS};
constexpr rtype::network::Wallet ALL_CHIPS_CASHED_OUT{.kopecks = SOME_CHIPS,
                                                      .chips = 0};

/** @brief Wallets whose chips, or kopecks, an exchange of AN_AMOUNT fills
 * exactly up to the largest balance. */
constexpr rtype::network::Wallet CHIPS_AN_AMOUNT_SHORT_OF_THE_LARGEST{
    .kopecks = SOME_KOPECKS, .chips = LARGEST_CHIPS - AN_AMOUNT};
constexpr rtype::network::Wallet KOPECKS_AN_AMOUNT_SHORT_OF_THE_LARGEST{
    .kopecks = LARGEST_KOPECKS - AN_AMOUNT, .chips = SOME_CHIPS};
