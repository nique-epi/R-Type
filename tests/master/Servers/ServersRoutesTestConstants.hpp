#pragma once

#include <string_view>

/** @brief The routes of a game server, as a client calls them. */
constexpr std::string_view OWN_SERVER_URL = "/api/servers/me";
constexpr std::string_view OWN_HEARTBEAT_URL = "/api/servers/me/heartbeat";

/** @brief The database file of the master, in the folder of the test. */
constexpr std::string_view ROUTES_DATABASE_FILE = "master.db";

/** @brief The name of the key issued before the master starts. */
constexpr std::string_view ROUTES_KEY_NAME = "Paris 1";

/** @brief A well-formed key the master never issued. */
constexpr std::string_view FOREIGN_KEY =
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief The headers the routes read and write. */
constexpr std::string_view AUTHORIZATION_NAME = "Authorization";
constexpr std::string_view BEARER_PREFIX_TEXT = "Bearer ";
constexpr std::string_view CHALLENGE_HEADER_NAME = "WWW-Authenticate";
constexpr std::string_view EXPECTED_CHALLENGE = "Bearer";
constexpr std::string_view LOCATION_NAME = "Location";
constexpr std::string_view EXPECTED_SERVER_LOCATION_PREFIX = "/api/servers/";
constexpr std::string_view JSON_TYPE_NAME = "application/json";
constexpr std::string_view CONTENT_TYPE_NAME = "Content-Type";
constexpr std::string_view PUT_METHOD_NAME = "PUT";

/** @brief The statuses the routes answer. */
constexpr int EXPECTED_ROUTE_OK = 200;
constexpr int EXPECTED_ROUTE_CREATED = 201;
constexpr int EXPECTED_ROUTE_BAD_REQUEST = 400;
constexpr int EXPECTED_ROUTE_UNAUTHORIZED = 401;
constexpr int EXPECTED_ROUTE_NOT_FOUND = 404;

/** @brief The members of the answers the tests read. */
constexpr std::string_view ID_MEMBER_NAME = "id";
constexpr std::string_view ADDRESS_MEMBER_NAME = "address";
constexpr std::string_view DETAIL_MEMBER_NAME = "detail";
constexpr std::string_view COMMANDS_MEMBER_NAME = "commands";

/** @brief The address of the TCP peer of every test, and a member to break. */
constexpr std::string_view PEER_ADDRESS = "127.0.0.1";
constexpr std::string_view ADDRESS_POINTER_NAME = "/address";
constexpr std::string_view QUOTED_UDP_PORT_POINTER = "\"/udpPort\"";
constexpr std::string_view UDP_PORT_POINTER_NAME = "/udpPort";
constexpr std::string_view PORT_WRITTEN_AS_TEXT = "4242";
