#pragma once

namespace rtype::network {

/** @brief The statuses of RFC 9110 the routes of an HttpServer answer. */
constexpr int OK_STATUS = 200;
constexpr int CREATED_STATUS = 201;
constexpr int NO_CONTENT_STATUS = 204;
constexpr int PARTIAL_CONTENT_STATUS = 206;
constexpr int BAD_REQUEST_STATUS = 400;
constexpr int UNAUTHORIZED_STATUS = 401;
constexpr int NOT_FOUND_STATUS = 404;
constexpr int CONFLICT_STATUS = 409;
constexpr int TOO_MANY_REQUESTS_STATUS = 429;

}  // namespace rtype::network
