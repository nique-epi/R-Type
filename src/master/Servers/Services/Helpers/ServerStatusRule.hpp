#pragma once

#include <chrono>
#include "ServerStatus.hpp"
#include "StatusEvidence.hpp"

namespace rtype::master {

/**
 * @returns The status @p evidence gives a registered server at @p now, the
 * first that applies: Maintenance when an admin took it out of service;
 * Offline when it was silent for OFFLINE_SILENCE or more; Degraded when the
 * 95th percentile of its ticks or its loss reaches its bound; else Online, a
 * measure not taken counting as within its bound. The silence is counted in
 * whole seconds, as the dates are kept.
 */
[[nodiscard]] network::ServerStatus statusOf(const StatusEvidence& evidence,
                                             std::chrono::sys_seconds now);

}  // namespace rtype::master
