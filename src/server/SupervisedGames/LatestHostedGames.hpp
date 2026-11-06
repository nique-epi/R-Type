#pragma once

#include <vector>
#include "HostedGame.hpp"
#include "LatestValue.hpp"

namespace rtype::server {

/**
 * @brief The games the supervisor runs as the master lists them, the newest
 * list: the supervised games module publishes it from the simulation thread
 * each time it changes, any thread reads it or waits for a newer one.
 */
using LatestHostedGames =
    concurrency::LatestValue<std::vector<network::HostedGame>>;

}  // namespace rtype::server
