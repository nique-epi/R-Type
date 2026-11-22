#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief Scores of the games of the tests, and what each pays: one kopeck
 * per 50 points, a part of a kopeck lost, and nothing more for a victory. */
constexpr std::uint32_t TWELVE_HUNDRED_THIRTY_FOUR_POINTS = 1'234;
constexpr std::int64_t KOPECKS_OF_1234_POINTS = 24;
constexpr std::uint32_t A_THOUSAND_POINTS = 1'000;
constexpr std::int64_t KOPECKS_OF_A_THOUSAND_POINTS = 20;
constexpr std::int64_t NO_KOPECK = 0;

/** @brief A ticket of the shape the master gives, which it never gave. */
constexpr std::string_view NEVER_GIVEN_TICKET =
    "rtt_0000000000000000000000000000000000000000000000000000000000000001";

/** @brief The second account of the tests, and its password. */
constexpr std::string_view BOB_NICKNAME = "BOB";
constexpr std::string_view BOB_PASSWORD = "another horse battery staple";

/** @brief A look ACE wins in a game, and the looks it then owns: the three of
 * the starter kit and that one. */
constexpr std::string_view WON_PARTY_HAT = "party_hat";
constexpr std::size_t LOOKS_WITH_THE_PARTY_HAT = 4;

/** @brief The member that holds the id of an account. */
constexpr std::string_view ACCOUNT_ID_MEMBER_NAME = "id";
