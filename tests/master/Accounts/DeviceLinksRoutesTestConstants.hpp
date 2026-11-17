#pragma once

#include <string_view>

/** @brief The paths of the routes of the device links. */
constexpr std::string_view DEVICE_LINKS_URL = "/api/device-links";
constexpr std::string_view OWN_DEVICE_LINK_URL = "/api/device-links/me";
constexpr std::string_view DEVICE_LINK_URL_START = "/api/device-links/";
constexpr std::string_view DECISION_URL_END = "/decision";

/** @brief The bodies of the decisions a player sends, and one that is no
 * decision. */
constexpr std::string_view APPROVAL_BODY = R"({"decision":"approved"})";
constexpr std::string_view DENIAL_BODY = R"({"decision":"denied"})";
constexpr std::string_view NO_DECISION_BODY = R"({"decision":"later"})";

/** @brief The members of a link and of its state, and the values the tests
 * expect of them. */
constexpr std::string_view SECRET_MEMBER_NAME = "secret";
constexpr std::string_view USER_CODE_MEMBER_NAME = "userCode";
constexpr std::string_view PENDING_STATUS_NAME = "pending";
constexpr std::string_view LINK_SECRET_START = "rtd_";

/** @brief A short code no link holds, the header that says how long to wait,
 * and the seconds it says. */
constexpr std::string_view UNKNOWN_USER_CODE = "BBBB-BBBB";
constexpr std::string_view RETRY_AFTER_HEADER_NAME = "Retry-After";
constexpr std::string_view FIVE_SECONDS_TEXT = "5";
