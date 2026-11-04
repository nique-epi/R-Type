#pragma once

#include <cstddef>
#include <string_view>

/** @brief The route a test adds, and the path a client calls it by. */
constexpr std::string_view TEST_ROUTE = "/thing";
constexpr std::string_view TEST_ROUTE_URL = "/api/thing";

/** @brief The body the test route answers. */
constexpr std::string_view TEST_ROUTE_BODY = R"({"answer":42})";

/** @brief The members the echo route answers what it read under. */
constexpr std::string_view ECHO_BODY_MEMBER = "body";
constexpr std::string_view ECHO_TOKEN_MEMBER = "token";
constexpr std::string_view ECHO_ADDRESS_MEMBER = "address";

/** @brief The body a test sends the echo route. */
constexpr std::string_view ECHOED_BODY = R"({"name":"Paris 1"})";

/** @brief The values the tests give the key header. */
constexpr std::string_view SENT_TOKEN = "4f2a9c";
constexpr std::string_view BEARER_AUTHORIZATION = "Bearer 4f2a9c";
constexpr std::string_view LOWERCASE_BEARER_AUTHORIZATION = "bearer 4f2a9c";
constexpr std::string_view BASIC_AUTHORIZATION = "Basic 4f2a9c";
constexpr std::string_view SHORT_AUTHORIZATION = "Bear";
constexpr std::string_view BARE_BEARER_AUTHORIZATION = "Bearer";

/** @brief A header that names another client than the TCP peer (the
 * loopback). */
constexpr std::string_view FORWARDED_FOR_HEADER = "X-Forwarded-For";
constexpr std::string_view FORGED_ADDRESS = "203.0.113.9";

/** @brief What a refusal of a test route tells the client. */
constexpr std::string_view BAD_REQUEST_DETAIL = "the name is empty";

/** @brief Where the thing a test route creates lives. */
constexpr std::string_view CREATED_LOCATION = "/api/thing/7";

/** @brief A path under /api that no test adds. */
constexpr std::string_view UNKNOWN_URL = "/api/nothing";

/** @brief The media type every error must carry. */
constexpr std::string_view EXPECTED_PROBLEM_TYPE = "application/problem+json";

/** @brief The members of a problem document of RFC 9457 the tests read. */
constexpr std::string_view PROBLEM_STATUS_MEMBER_NAME = "status";
constexpr std::string_view PROBLEM_TITLE_MEMBER_NAME = "title";

/** @brief The statuses of the errors the tests expect, with their phrase. */
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
