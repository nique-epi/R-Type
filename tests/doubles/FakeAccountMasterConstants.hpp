#pragma once

#include <string_view>

/** @brief Where FakeAccountMaster listens: the loopback, on a port the system
 * chooses. */
constexpr std::string_view ACCOUNT_MASTER_HOST = "127.0.0.1";
constexpr std::string_view ACCOUNT_MASTER_SCHEME = "http://";

/** @brief What it answers a route no test gave an answer: 404, as the master
 * answers a path it does not serve. */
constexpr int UNANSWERED_ROUTE_STATUS = 404;

/** @brief The paths it serves, as cpp-httplib matches them: every one. */
constexpr std::string_view EVERY_PATH = ".*";

/** @brief What separates the method and the path of a route it answers:
 * `GET /api/users/me`. */
constexpr std::string_view ROUTE_METHOD_SEPARATOR = " ";

/** @brief The type of the body of an answer. */
constexpr std::string_view ACCOUNT_MASTER_CONTENT_TYPE = "application/json";

/** @brief What separates the host and the port of its url. */
constexpr std::string_view ACCOUNT_MASTER_PORT_SEPARATOR = ":";

/** @brief The header a client sends its token in. */
constexpr std::string_view AUTHORIZATION_HEADER_NAME = "Authorization";
