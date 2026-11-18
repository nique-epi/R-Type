#pragma once

#include <httplib.h>
#include <string>
#include "MasterRoute.hpp"

namespace rtype::network {

/** @returns The request of @p route for @p path, the path of the route with
 * its parameters written in (apiPathOf(), roomPathOf()…), with no body. */
[[nodiscard]] httplib::Request masterRequestOf(MasterRoute route,
                                               std::string path);

/** @returns The request of @p route for @p path, as masterRequestOf(), with
 * @p body as its body, typed JSON_CONTENT_TYPE. */
[[nodiscard]] httplib::Request jsonRequestOf(MasterRoute route,
                                             std::string path,
                                             std::string body);

}  // namespace rtype::network
