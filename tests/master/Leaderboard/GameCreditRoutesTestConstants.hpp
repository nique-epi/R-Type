#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief Scores of the games of the tests, and what each pays: one kopeck
 * per 10 points, and a quarter more for a victory. */
constexpr std::uint32_t TWELVE_HUNDRED_THIRTY_FOUR_POINTS = 1'234;
constexpr std::int64_t KOPECKS_OF_1234_POINTS = 123;
constexpr std::uint32_t A_THOUSAND_POINTS = 1'000;
constexpr std::int64_t KOPECKS_OF_A_THOUSAND_POINTS_WON = 125;
constexpr std::int64_t NO_KOPECK = 0;

/** @brief Account ids no account of the tests has: a text that is no number,
 * and a number past every account. */
constexpr std::string_view NO_NUMBER_ACCOUNT_ID = "ACE";
constexpr std::string_view NO_SUCH_ACCOUNT_ID = "999999";

/** @brief A look ACE wins in a game, and the looks it then owns: the three of
 * the starter kit and that one. */
constexpr std::string_view WON_PARTY_HAT = "party_hat";
constexpr std::size_t LOOKS_WITH_THE_PARTY_HAT = 4;

/** @brief The member that holds the id of an account. */
constexpr std::string_view ACCOUNT_ID_MEMBER_NAME = "id";
