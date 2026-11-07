#pragma once

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

}  // namespace rtype::master
