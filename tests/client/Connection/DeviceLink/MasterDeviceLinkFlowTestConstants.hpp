#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief How long a test waits for the flow to publish what it expects. */
constexpr std::chrono::seconds FLOW_READING_DEADLINE{10};

/** @brief The routes the flow calls, as the fake master names them. */
constexpr std::string_view CREATE_LINK_ROUTE = "POST /api/device-links";
constexpr std::string_view READ_OWN_LINK_ROUTE = "GET /api/device-links/me";
constexpr std::string_view DELETE_OWN_LINK_ROUTE =
    "DELETE /api/device-links/me";
constexpr std::string_view CREATE_SESSION_ROUTE = "POST /api/sessions";
constexpr std::string_view DELETE_METHOD_NAME = "DELETE";
constexpr std::string_view OWN_LINK_PATH = "/api/device-links/me";
constexpr std::string_view SECRET_START = "rtd_";

/** @brief The statuses the fake master answers with. */
constexpr int FLOW_OK_STATUS = 200;
constexpr int FLOW_CREATED_STATUS = 201;
constexpr int FLOW_TOO_MANY_REQUESTS_STATUS = 429;

/** @brief The link the fake master opens: its secret, its code, its page,
 * its dates, and an interval of one second, so a test waits little. */
constexpr std::string_view FLOW_SECRET =
    "rtd_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view FLOW_BEARER =
    "Bearer "
    "rtd_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view FLOW_USER_CODE = "KQFB-WZPM";
constexpr std::string_view FLOW_PAGE = "https://rtype.app/link";
constexpr std::string_view FLOW_PAGE_WITH_CODE =
    "https://rtype.app/link?code=KQFB-WZPM";
constexpr std::chrono::sys_seconds FLOW_CREATED_AT{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds FLOW_EXPIRES_AT{
    std::chrono::seconds{1'800'000'600}};
constexpr std::chrono::seconds FLOW_INTERVAL{1};

/** @brief The token of the session the approved link is traded for, and the
 * account it belongs to. */
constexpr std::string_view FLOW_TOKEN =
    "rts_fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210";
constexpr std::int64_t FLOW_ACE_ID = 7;
constexpr std::string_view FLOW_ACE_NICKNAME = "ACE";

}  // namespace rtype::client::testing
