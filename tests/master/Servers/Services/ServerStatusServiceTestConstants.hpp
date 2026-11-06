#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "MasterMessageTextTestConstants.hpp"
#include "MasterRouteConstants.hpp"
#include "PageRequest.hpp"

/** @brief Later dates of a test, after FIRST_HEARTBEAT_DATE. */
constexpr std::chrono::seconds FIVE_SECONDS{5};
constexpr std::chrono::seconds FIFTEEN_SECONDS{15};

/** @brief A tick p95 over one tick (16.67 ms). */
constexpr double SLOW_TICK_P95 = 20.0;

/** @brief Makes the database refuse every write of a status, as a full disk
 * refuses a write. */
constexpr std::string_view REFUSE_STATUS_WRITES_TRIGGER = R"sql(
CREATE TRIGGER refuse_status_writes BEFORE UPDATE OF status ON servers
BEGIN SELECT RAISE(ABORT, 'refused'); END
)sql";

/** @brief Another UDP port than the one "Paris 1" registers with. */
constexpr std::uint16_t OTHER_UDP_PORT =
    static_cast<std::uint16_t>(rtype::network::testing::PARIS_UDP_PORT + 1);

/**
 * @brief Records an answer to a probe at the second each server registered:
 * the answer a heartbeat brings, without the measures it brings, which only
 * the database can give.
 */
constexpr std::string_view ANSWER_AT_THE_REGISTRATION_STATEMENT =
    "UPDATE servers SET last_probe_answer_at = registered_at";

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
