#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "CasinoController.hpp"
#include "MasterRoute.hpp"

namespace rtype::master {

/** @brief A route of the casino and the member of CasinoController that
 * answers it. */
struct CasinoRoute {
  network::MasterRoute route;
  network::ApiResponse (CasinoController::*answer)(const network::ApiRequest&){
      nullptr};
};

}  // namespace rtype::master
