#pragma once

#include <memory>
#include <vector>
#include "IGameModule.hpp"

namespace rtype::server {

/**
 * @returns The modules of a game of R-Type, in the order a game installs them:
 * the replication, which provides what the rules require, then the rules. A
 * game of another game lists its own.
 */
[[nodiscard]] std::vector<std::unique_ptr<IGameModule>> rtypeGameModules();

}  // namespace rtype::server
