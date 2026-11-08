#include "MasterRoutePath.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include "MasterHttpConstants.hpp"
#include "MasterRoute.hpp"
#include "MasterRouteConstants.hpp"
#include "RouteMethod.hpp"

namespace rtype::network {

namespace {

/** @brief A parameter of the path of a route, and the text it stands for. */
struct RouteParameter {
  std::string_view name;
  std::string_view value;
};

/** @returns The path of @p route with @p parameter in place of its name. */
std::string withPathParameter(MasterRoute route, RouteParameter parameter) {
  std::string path = apiPathOf(route);
  const std::string placeholder =
      std::string{PATH_PARAMETER_PREFIX} + std::string{parameter.name};
  path.replace(path.find(placeholder), placeholder.size(), parameter.value);
  return path;
}

}  // namespace

std::string apiPathOf(MasterRoute route) {
  return std::string{MASTER_API_PREFIX} + std::string{route.path};
}

std::string memberPathOf(MasterRoute route, std::int64_t memberId) {
  return apiPathOf(route) + std::string{PATH_SEPARATOR} +
         std::to_string(memberId);
}

std::string serverPathOf(MasterRoute route, std::int64_t serverId) {
  const std::string serverIdText = std::to_string(serverId);
  return withPathParameter(
      route,
      RouteParameter{.name = SERVER_ID_PATH_PARAMETER, .value = serverIdText});
}

std::string gamePathOf(MasterRoute route, std::string_view gameId) {
  return withPathParameter(
      route, RouteParameter{.name = GAME_ID_PATH_PARAMETER, .value = gameId});
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
