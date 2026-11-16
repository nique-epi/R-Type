#pragma once

#include <string>
#include "MasterRoute.hpp"

namespace httplib {
struct Request;
}  // namespace httplib

namespace rtype::network {

/** @returns The request of @p route for @p path, the path of the route with
 * its parameters written in (apiPathOf(), roomPathOf()…), with no body. */
[[nodiscard]] httplib::Request masterRequestOf(MasterRoute route,
                                               std::string path);

/** @brief Makes @p body the body of @p request, typed JSON_CONTENT_TYPE. */
void setJsonBody(httplib::Request& request, std::string body);

}  // namespace rtype::network
