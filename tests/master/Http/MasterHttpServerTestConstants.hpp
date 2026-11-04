#pragma once

#include <cstddef>
#include <string_view>

/** @brief The address every test server listens on and every client calls. */
constexpr std::string_view LOOPBACK_ADDRESS = "127.0.0.1";

/** @brief The route a test adds, and the path a client calls it by. */
constexpr std::string_view TEST_ROUTE = "/thing";
constexpr std::string_view TEST_ROUTE_URL = "/api/thing";

/** @brief The body the test route answers. */
constexpr std::string_view TEST_ROUTE_BODY = R"({"answer":42})";

/** @brief A path under /api that no test adds. */
constexpr std::string_view UNKNOWN_URL = "/api/nothing";

/** @brief The media types every answer and every error must carry. */
constexpr std::string_view EXPECTED_JSON_TYPE = "application/json";
constexpr std::string_view EXPECTED_PROBLEM_TYPE = "application/problem+json";

/** @brief The members of a problem document of RFC 9457 the tests read. */
constexpr std::string_view PROBLEM_STATUS_MEMBER_NAME = "status";
constexpr std::string_view PROBLEM_TITLE_MEMBER_NAME = "title";

/** @brief The statuses the tests expect, with their standard phrase. */
constexpr int EXPECTED_OK = 200;
constexpr int EXPECTED_NOT_FOUND = 404;
constexpr std::string_view EXPECTED_NOT_FOUND_TITLE = "Not Found";
constexpr int EXPECTED_PAYLOAD_TOO_LARGE = 413;
constexpr int EXPECTED_INTERNAL_SERVER_ERROR = 500;
constexpr std::string_view EXPECTED_INTERNAL_SERVER_ERROR_TITLE =
    "Internal Server Error";

/** @brief What a route that fails throws, which no client may read. */
constexpr std::string_view SECRET_REASON = "the database file is on fire";

/** @brief One byte more than the 64 KiB a request body may hold. */
constexpr std::size_t OVERSIZED_BODY_BYTES = (std::size_t{64} * 1024) + 1;

/** @brief The method and the media type of the oversized request a test
 * sends to its route. */
constexpr std::string_view GET_METHOD = "GET";
constexpr std::string_view TEXT_CONTENT_TYPE = "text/plain";

/** @brief The header of an answer that names its media type. */
constexpr std::string_view CONTENT_TYPE_HEADER = "Content-Type";
