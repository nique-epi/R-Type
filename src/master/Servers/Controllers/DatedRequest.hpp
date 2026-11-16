#pragma once

#include <chrono>
#include <functional>
#include "ApiRequest.hpp"

namespace rtype::master {

/** @brief A request a route answers, and the second it answers it at: what a
 * route that reads a session needs to name its caller. */
struct DatedRequest {
  std::reference_wrapper<const network::ApiRequest> request;
  std::chrono::sys_seconds now;
};

}  // namespace rtype::master
