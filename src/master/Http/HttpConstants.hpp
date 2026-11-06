#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::master {

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

/** @brief The headers only the master writes (see MasterHttpConstants.hpp). */
constexpr std::string_view LOCATION_HEADER = "Location";
constexpr std::string_view WWW_AUTHENTICATE_HEADER = "WWW-Authenticate";

/** @brief The path the web site is served under, outside the API. */
constexpr std::string_view SITE_MOUNT_POINT = "/";

/**
 * @brief The page of the web site a path outside the API that no file answers
 * gets, in the folder of the site, as Next.js writes it.
 */
constexpr std::string_view NOT_FOUND_PAGE_FILE_NAME = "404.html";

/** @brief The type of NOT_FOUND_PAGE_FILE_NAME. */
constexpr std::string_view HTML_CONTENT_TYPE = "text/html";

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
