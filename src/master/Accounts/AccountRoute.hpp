#pragma once

#include "AccountsController.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "MasterRoute.hpp"

namespace rtype::master {

/** @brief A route of the accounts and the member of AccountsController that
 * answers it. */
struct AccountRoute {
  network::MasterRoute route;
  network::ApiResponse (AccountsController::*answer)(
      const network::ApiRequest&){nullptr};
};

}  // namespace rtype::master
