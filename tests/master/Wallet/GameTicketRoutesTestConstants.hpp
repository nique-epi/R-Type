#pragma once

#include <cstddef>
#include <string_view>

/** @brief The paths of the game tickets the tests call. */
constexpr std::string_view OWN_TICKETS_URL = "/api/users/me/tickets";
constexpr std::string_view TICKET_LOCATION_START = "/api/users/me/tickets/";
constexpr std::string_view UNKNOWN_TICKET_URL =
    "/api/users/me/tickets/"
    "0000000000000000000000000000000000000000000000000000000000000000";

/** @brief The members of the answers about a ticket, and how an unused one
 * is written. */
constexpr std::string_view TICKET_MEMBER_NAME = "ticket";
constexpr std::string_view EXPIRES_AT_MEMBER_NAME = "expiresAt";
constexpr std::string_view UNUSED_MEMBER_TEXT = R"("used":false)";

/** @brief The size of `rtt_`, before the random part of a ticket. */
constexpr std::size_t TICKET_PREFIX_SIZE = 4;
