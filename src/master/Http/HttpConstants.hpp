#pragma once

#include <string_view>
#include "HttpStatusConstants.hpp"

namespace rtype::master {

/** @brief The member of the answer of network::HEALTH_ROUTE that holds the
 * version. */
constexpr std::string_view VERSION_MEMBER = "version";

/** @brief The statuses the routes of the master answer. */
constexpr int OK_STATUS = network::OK_STATUS;
constexpr int CREATED_STATUS = network::CREATED_STATUS;
constexpr int BAD_REQUEST_STATUS = network::BAD_REQUEST_STATUS;
constexpr int UNAUTHORIZED_STATUS = network::UNAUTHORIZED_STATUS;
constexpr int NOT_FOUND_STATUS = network::NOT_FOUND_STATUS;
constexpr int CONFLICT_STATUS = network::CONFLICT_STATUS;
constexpr int TOO_MANY_REQUESTS_STATUS = network::TOO_MANY_REQUESTS_STATUS;

/** @brief The headers only the master writes (see MasterHttpConstants.hpp). */
constexpr std::string_view LOCATION_HEADER = "Location";
constexpr std::string_view WWW_AUTHENTICATE_HEADER = "WWW-Authenticate";

}  // namespace rtype::master
