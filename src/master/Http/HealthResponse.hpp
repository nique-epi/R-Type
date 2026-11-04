#pragma once

#include "ApiResponse.hpp"

namespace rtype::master {

/**
 * @returns The answer of GET /api/health: OK_STATUS and
 * `{"version":"<MASTER_VERSION>"}`. It reads nothing else, so it answers as
 * long as the HTTP server runs.
 */
[[nodiscard]] ApiResponse healthResponse();

}  // namespace rtype::master
