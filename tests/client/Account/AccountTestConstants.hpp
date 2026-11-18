#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "Wallet.hpp"

namespace rtype::client::testing {

/** @brief Two tokens of sessions, the first one of ACE. */
constexpr std::string_view ACE_TOKEN =
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view OTHER_TOKEN =
    "rt_fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";

/** @brief The account of ACE_TOKEN, and its id as the game server reads it. */
constexpr std::int64_t ACE_ID = 7;
constexpr std::uint64_t ACE_ACCOUNT_ID = 7;
constexpr std::string_view ACE_ACCOUNT_NICKNAME = "ACE";

/** @brief The first ask of each kind a link numbers, and the second. */
constexpr std::uint64_t FIRST_ASK = 1;
constexpr std::uint64_t SECOND_ASK = 2;

/** @brief The balances of the wallet of ACE; it owns the crown alone
 * (crownOnlyLooks(), AccountFixtures.hpp). */
constexpr network::Wallet ACE_BALANCES{.kopecks = 1250, .chips = 40};

/** @brief Where the crown sits in identifiers::SHIP_LOOK_ITEM_NAMES, and its
 * name. */
constexpr std::size_t CROWN_ITEM = 2;
constexpr std::string_view CROWN_NAME = "crown";

/** @brief 500 kopecks changed into chips. */
constexpr network::ExchangeRequest FIVE_HUNDRED_CHIPS{
    .from = network::Currency::Kopecks, .amount = 500};

}  // namespace rtype::client::testing
