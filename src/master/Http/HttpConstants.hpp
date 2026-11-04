#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::master {

/** @brief The media type of every answer of a route. */
constexpr std::string_view JSON_CONTENT_TYPE = "application/json";

/**
 * @brief The media type of an error, a problem document of RFC 9457, whose
 * `title` is the phrase of its status.
 */
constexpr std::string_view PROBLEM_CONTENT_TYPE = "application/problem+json";

/** @brief The members of a problem document of RFC 9457 the master writes. */
constexpr std::string_view PROBLEM_TITLE_MEMBER = "title";
constexpr std::string_view PROBLEM_STATUS_MEMBER = "status";
constexpr std::string_view PROBLEM_DETAIL_MEMBER = "detail";

/** @brief The member of the answer of network::HEALTH_ROUTE that holds the
 * version. */
constexpr std::string_view VERSION_MEMBER = "version";

/** @brief The statuses the routes of the master answer. */
constexpr int OK_STATUS = 200;
constexpr int CREATED_STATUS = 201;
constexpr int BAD_REQUEST_STATUS = 400;
constexpr int UNAUTHORIZED_STATUS = 401;
constexpr int NOT_FOUND_STATUS = 404;
constexpr int CONFLICT_STATUS = 409;
constexpr int TOO_MANY_REQUESTS_STATUS = 429;

/** @brief The lowest status of an error, which gets a problem document. */
constexpr int LOWEST_ERROR_STATUS = 400;

/** @brief The headers the master reads and writes. */
constexpr std::string_view AUTHORIZATION_HEADER = "Authorization";
constexpr std::string_view LOCATION_HEADER = "Location";
constexpr std::string_view WWW_AUTHENTICATE_HEADER = "WWW-Authenticate";

/**
 * @brief The scheme of RFC 6750 a key is sent with, read in any case as RFC
 * 9110 asks, and what an answer of UNAUTHORIZED_STATUS asks for.
 */
constexpr std::string_view BEARER_SCHEME = "Bearer";
constexpr std::string_view BEARER_SCHEME_SEPARATOR = " ";

/** @brief What cpp-httplib gives in place of a port it could not bind. */
constexpr int NO_PORT_BOUND = -1;

/**
 * @brief The largest body of a request a route reads, 64 KiB: a heartbeat of
 * a server is a few KiB, and a larger body gets 413 without being kept whole,
 * where cpp-httplib would keep up to 100 MiB. A path no route answers gets 404
 * and its body is never read.
 */
constexpr std::size_t MAX_REQUEST_BODY_BYTES = std::size_t{64} * 1024;

/** @brief What the journal says when the HTTP server stops on its own. */
constexpr std::string_view LISTENING_LOST_TEXT =
    "the HTTP server stopped listening";

/** @brief What the journal says of an exception that is no std::exception. */
constexpr std::string_view UNKNOWN_ERROR_TEXT = "unknown error";

}  // namespace rtype::master
