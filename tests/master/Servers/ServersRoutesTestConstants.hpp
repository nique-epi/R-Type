#pragma once

#include <string_view>

/** @brief The routes of a game server, as a client calls them. */
constexpr std::string_view OWN_SERVER_URL = "/api/servers/me";
constexpr std::string_view OWN_HEARTBEAT_URL = "/api/servers/me/heartbeat";

/** @brief How a key is sent, and the challenge of an answer without one. */
constexpr std::string_view BEARER_PREFIX_TEXT = "Bearer ";
constexpr std::string_view CHALLENGE_HEADER_NAME = "WWW-Authenticate";
constexpr std::string_view EXPECTED_CHALLENGE = "Bearer";

/** @brief Where a registered server lives, followed by its id. */
constexpr std::string_view EXPECTED_SERVER_LOCATION_PREFIX = "/api/servers/";

/** @brief The members of the answers the tests read. */
constexpr std::string_view ID_MEMBER_NAME = "id";
constexpr std::string_view ADDRESS_MEMBER_NAME = "address";
constexpr std::string_view COMMANDS_MEMBER_NAME = "commands";
