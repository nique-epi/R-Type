#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "MasterRouteConstants.hpp"
#include "PageRequest.hpp"

/** @brief When the server of a test registers, and later dates. */
constexpr std::chrono::sys_seconds REGISTRATION_DATE{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::seconds FIVE_SECONDS{5};
constexpr std::chrono::seconds FIFTEEN_SECONDS{15};
constexpr std::chrono::seconds EIGHT_WHOLE_DAYS{8 * 86'400};

/** @brief A tick p95 over one tick (16.67 ms), and the quiet measures of
 * "Paris 1". */
constexpr double SLOW_TICK_P95 = 20.0;
constexpr double QUIET_TICK_P95_OF_PARIS = 4.25;
constexpr double QUIET_LOSS_OF_PARIS = 0.5;

/** @brief An id no server has. */
constexpr std::int64_t UNKNOWN_SERVER_ID = 999;

/** @brief The start of the address each server of a test is created from,
 * and the address of a server that never registers. */
constexpr std::string_view STATUS_CREATOR_PREFIX = "192.0.2.";
constexpr std::string_view NEVER_REGISTERED_CREATOR = "198.51.100.9";

/** @brief Writes the last heartbeat of a server, as an older master kept it. */
constexpr std::string_view KEEP_HEARTBEAT_TEXT_STATEMENT =
    "UPDATE servers SET last_heartbeat = :text WHERE id = :id";
constexpr std::string_view KEPT_TEXT_PARAMETER = ":text";
constexpr std::string_view KEPT_ID_PARAMETER = ":id";

/** @brief A heartbeat this master cannot read: it has no `games`. */
constexpr std::string_view UNREADABLE_HEARTBEAT_TEXT = "{}";

/** @brief The servers a test of the pages registers, and the page it reads. */
constexpr int THREE_SERVERS = 3;
constexpr std::int64_t ONE_ITEM = 1;
constexpr std::int64_t AFTER_THE_FIRST = 1;
constexpr std::size_t SECOND_SERVER = 1;

/** @brief Every item of a list, up to the largest page. */
constexpr rtype::master::PageRequest WHOLE_PAGE{
    .limit = rtype::network::MAXIMUM_PAGE_SIZE,
    .offset = rtype::network::FIRST_PAGE_OFFSET};
