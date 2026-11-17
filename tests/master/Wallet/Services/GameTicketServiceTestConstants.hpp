#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief How the tests count the rows of `game_tickets` keyed by a text. */
constexpr std::string_view TICKET_ROWS_QUERY =
    "SELECT COUNT(*) FROM game_tickets WHERE fingerprint = :fingerprint";
constexpr std::string_view TICKET_FINGERPRINT_TEST_PARAMETER = ":fingerprint";

/** @brief How long a ticket pays, as the master is required to keep it: the
 * lifetime the route documents, 6 hours. */
constexpr std::chrono::seconds SIX_HOURS = std::chrono::hours{6};

/** @brief The tickets one account may be given in an hour, and an hour and a
 * second, after which the first of them no longer counts. */
constexpr std::size_t TICKETS_OF_AN_HOUR = 30;
constexpr std::chrono::seconds JUST_OVER_AN_HOUR =
    std::chrono::hours{1} + std::chrono::seconds{1};
