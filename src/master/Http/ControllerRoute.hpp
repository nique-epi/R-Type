#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "MasterHttpServer.hpp"
#include "MasterRoute.hpp"

namespace rtype::master {

/** @brief A route of the master and the member of @p Controller that
 * answers it. */
template <typename Controller>
struct ControllerRoute {
  network::MasterRoute route;
  network::ApiResponse (Controller::*answer)(const network::ApiRequest&){
      nullptr};
};

/** @brief Adds every route of @p routes to @p server, answered by
 * @p controller, which must outlive the server. */
template <typename Controller, std::size_t Count>
void addRoutes(MasterHttpServer& server,
               const std::array<ControllerRoute<Controller>, Count>& routes,
               Controller& controller) {
  for (const ControllerRoute<Controller>& controllerRoute : routes) {
    server.add(controllerRoute.route,
               std::bind_front(controllerRoute.answer, &controller));
  }
}

}  // namespace rtype::master
