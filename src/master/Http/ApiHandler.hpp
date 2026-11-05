#pragma once

#include <functional>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"

namespace rtype::master {

/**
 * @brief What answers a route of the API, on a thread of the HTTP server: it
 * may throw, and the client then gets a 500 that does not tell why.
 */
using ApiHandler = std::function<ApiResponse(const ApiRequest&)>;

}  // namespace rtype::master
