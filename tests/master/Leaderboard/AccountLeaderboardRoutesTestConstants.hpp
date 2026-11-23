#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "LeaderboardRoutesTestConstants.hpp"

/** @brief The route of the leaderboard of the accounts, as a client calls
 * it. */
constexpr std::string_view ACCOUNT_LEADERBOARD_URL =
    "/api/leaderboard/accounts";

/** @brief The members of its answer the tests read. */
constexpr std::string_view ACCOUNTS_MEMBER_NAME = "accounts";
constexpr std::string_view LAST_PLAYED_AT_MEMBER_NAME = "lastPlayedAt";

/** @brief What an account totals with a game of 1234 points and a game of
 * 1000: 2234. */
constexpr std::int64_t TOTAL_OF_1234_AND_A_THOUSAND = 2'234;

/** @brief A score above each of those two games and below their total. */
constexpr std::uint32_t TWO_THOUSAND_POINTS = 2'000;

/** @brief A game that ended a day after GAME_DATE, and that second. */
constexpr std::chrono::sys_seconds LATER_GAME_DATE =
    GAME_DATE + std::chrono::days{1};
constexpr std::int64_t LATER_GAME_SECOND = 1'800'086'400;

/** @brief The rank of the highest total. */
constexpr std::int64_t BEST_RANK = 1;

/** @brief Queries of the leaderboard of the accounts, after its path: its
 * first page, its second account alone, and a page of no row, which the
 * master refuses. */
constexpr std::string_view FIRST_PAGE_OF_ACCOUNTS_QUERY;
constexpr std::string_view SECOND_ACCOUNT_ALONE_QUERY = "?limit=1&offset=1";
constexpr std::string_view PAGE_OF_NO_ROW_QUERY = "?limit=0";
