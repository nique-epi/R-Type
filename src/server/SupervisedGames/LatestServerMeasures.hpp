#pragma once

#include "LatestValue.hpp"
#include "ServerMeasures.hpp"

namespace rtype::server {

/**
 * @brief The newest measures of the games the supervisor runs, as the master
 * reads them: the supervised games module publishes them from the simulation
 * thread each time they change, any thread reads them.
 */
using LatestServerMeasures = concurrency::LatestValue<network::ServerMeasures>;

}  // namespace rtype::server
