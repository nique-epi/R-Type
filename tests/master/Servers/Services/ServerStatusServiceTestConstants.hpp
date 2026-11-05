#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "MasterRouteConstants.hpp"
#include "PageRequest.hpp"

/** @brief Later dates of a test, after FIRST_HEARTBEAT_DATE. */
constexpr std::chrono::seconds FIVE_SECONDS{5};
constexpr std::chrono::seconds FIFTEEN_SECONDS{15};

/** @brief A tick p95 over one tick (16.67 ms). */
constexpr double SLOW_TICK_P95 = 20.0;

/** @brief An id no server has. */
constexpr std::int64_t UNKNOWN_SERVER_ID = 999;

/** @brief The address of a server that never registers. */
constexpr std::string_view NEVER_REGISTERED_CREATOR = "198.51.100.9";

/** @brief The servers a test of the pages registers, and the page it reads. */
constexpr int THREE_SERVERS = 3;
constexpr std::int64_t ONE_ITEM = 1;
constexpr std::int64_t AFTER_THE_FIRST = 1;
constexpr std::size_t SECOND_SERVER = 1;

/** @brief Every item of a list, up to the largest page. */
constexpr rtype::master::PageRequest WHOLE_PAGE{
    .limit = rtype::network::MAXIMUM_PAGE_SIZE,
    .offset = rtype::network::FIRST_PAGE_OFFSET};
