#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::master {

/** @brief The path every route of the API of the master starts with. */
constexpr std::string_view API_PATH_PREFIX = "/api";

/** @brief The route that says the master answers, and in which version. */
constexpr std::string_view HEALTH_PATH = "/health";

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

/** @brief The member of the answer of HEALTH_PATH that holds the version. */
constexpr std::string_view VERSION_MEMBER = "version";

/** @brief The status of an answer that succeeded. */
constexpr int OK_STATUS = 200;

/** @brief The lowest status of an error: 400 and above are errors. */
constexpr int FIRST_ERROR_STATUS = 400;

/** @brief What cpp-httplib gives in place of a port it could not bind. */
constexpr int NO_PORT_BOUND = -1;

/**
 * @brief The largest body of a request a route reads, 64 KiB: a heartbeat of
 * a server is a few KiB, and a larger body gets 413 without being kept whole,
 * where cpp-httplib would keep up to 100 MiB. A path no route answers gets 404
 * and its body is never read.
 */
constexpr std::size_t MAX_REQUEST_BODY_BYTES = std::size_t{64} * 1024;

/**
 * @brief The address the master listens on: every interface of the machine,
 * as the server does, so servers and players of the local network reach it.
 */
constexpr std::string_view ALL_INTERFACES = "0.0.0.0";

/** @brief What the journal says when the HTTP server stops on its own. */
constexpr std::string_view LISTENING_LOST_TEXT =
    "the HTTP server stopped listening";

/** @brief What the journal says of an exception that is no std::exception. */
constexpr std::string_view UNKNOWN_ERROR_TEXT = "unknown error";

}  // namespace rtype::master
