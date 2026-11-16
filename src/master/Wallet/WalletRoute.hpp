#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "MasterRoute.hpp"
#include "WalletController.hpp"

namespace rtype::master {

/** @brief A route of the wallets and the member of WalletController that
 * answers it. */
struct WalletRoute {
  network::MasterRoute route;
  network::ApiResponse (WalletController::*answer)(const network::ApiRequest&){
      nullptr};
};

}  // namespace rtype::master
