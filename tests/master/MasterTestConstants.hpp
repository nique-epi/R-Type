#pragma once

#include <string_view>

/** @brief The statuses the routes of the master answer in the tests. */
constexpr int EXPECTED_OK = 200;
constexpr int EXPECTED_CREATED = 201;
constexpr int EXPECTED_BAD_REQUEST = 400;
constexpr int EXPECTED_UNAUTHORIZED = 401;
constexpr int EXPECTED_NOT_FOUND = 404;
constexpr int EXPECTED_CONFLICT = 409;
constexpr int EXPECTED_TOO_MANY_REQUESTS = 429;

/** @brief The headers, the method and the media type the tests use. */
constexpr std::string_view AUTHORIZATION_HEADER_NAME = "Authorization";
constexpr std::string_view LOCATION_HEADER_NAME = "Location";
constexpr std::string_view CONTENT_TYPE_HEADER = "Content-Type";
constexpr std::string_view EXPECTED_JSON_TYPE = "application/json";
constexpr std::string_view PUT_METHOD = "PUT";

/** @brief The member of a problem document that says what was refused. */
constexpr std::string_view PROBLEM_DETAIL_MEMBER_NAME = "detail";

/** @brief The address every test client connects from. */
constexpr std::string_view LOOPBACK_ADDRESS = "127.0.0.1";

/** @brief The database file the tests give the master, in their folder. */
constexpr std::string_view TEST_DATABASE_FILE = "master.db";

/** @brief The game server the tests create and register. */
constexpr std::string_view TEST_SERVER_NAME = "Paris 1";

/** @brief A key shaped like those the master issues, which it never issued. */
constexpr std::string_view UNKNOWN_SERVER_KEY =
    "rtk_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
