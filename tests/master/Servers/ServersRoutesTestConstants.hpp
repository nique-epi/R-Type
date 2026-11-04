#pragma once

#include <cstddef>
#include <string_view>

/** @brief The routes of the game servers, as a client calls them. */
constexpr std::string_view SERVERS_URL = "/api/servers";
constexpr std::string_view OWN_SERVER_URL = "/api/servers/me";
constexpr std::string_view OWN_HEARTBEAT_URL = "/api/servers/me/heartbeat";
constexpr std::string_view POST_METHOD = "POST";

/** @brief How a key is sent, and the challenge of an answer without one. */
constexpr std::string_view BEARER_PREFIX_TEXT = "Bearer ";
constexpr std::string_view CHALLENGE_HEADER_NAME = "WWW-Authenticate";
constexpr std::string_view EXPECTED_CHALLENGE = "Bearer";

/** @brief Where a created server lives, followed by its id. */
constexpr std::string_view EXPECTED_SERVER_LOCATION_PREFIX = "/api/servers/";

/** @brief The body of a creation, and one that names no server. */
constexpr std::string_view PARIS_CREATION_BODY = R"({"name":"Paris 1"})";
constexpr std::string_view EMPTY_OBJECT_BODY = "{}";
constexpr std::string_view NAME_POINTER_TEXT = "/name";

/** @brief The creations one address may make in an hour, the fixture's first
 * among them. */
constexpr std::size_t CREATIONS_PER_HOUR = 5;

/** @brief The members of the answers the tests read. */
constexpr std::string_view ID_MEMBER_NAME = "id";
constexpr std::string_view KEY_MEMBER_NAME = "key";
constexpr std::string_view ADDRESS_MEMBER_NAME = "address";
constexpr std::string_view COMMANDS_MEMBER_NAME = "commands";
