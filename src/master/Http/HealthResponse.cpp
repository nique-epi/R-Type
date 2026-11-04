#include "HealthResponse.hpp"
#include <nlohmann/json.hpp>
#include "ApiResponse.hpp"
#include "HttpConstants.hpp"
#include "MasterConstants.hpp"

namespace rtype::master {

ApiResponse healthResponse() {
  nlohmann::json health;
  health[VERSION_MEMBER] = MASTER_VERSION;
  return ApiResponse{.status = OK_STATUS, .body = health.dump()};
}

}  // namespace rtype::master
