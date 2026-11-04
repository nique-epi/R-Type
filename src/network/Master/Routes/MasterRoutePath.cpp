#include "MasterRoutePath.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include "MasterHttpConstants.hpp"
#include "MasterRoute.hpp"
#include "MasterRouteConstants.hpp"
#include "RouteMethod.hpp"

namespace rtype::network {

std::string apiPathOf(MasterRoute route) {
  return std::string{MASTER_API_PREFIX} + std::string{route.path};
}

std::string memberPathOf(MasterRoute route, std::int64_t memberId) {
  return apiPathOf(route) + std::string{PATH_SEPARATOR} +
         std::to_string(memberId);
}

std::string_view methodNameOf(RouteMethod method) {
  switch (method) {
    case RouteMethod::Get:
      return GET_METHOD;
    case RouteMethod::Post:
      return POST_METHOD;
    case RouteMethod::Put:
      return PUT_METHOD;
  }
  return GET_METHOD;
}

}  // namespace rtype::network
