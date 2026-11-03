#pragma once

#include <memory>
#include <vector>
#include "IGameModule.hpp"
#include "IResultsArchive.hpp"

namespace rtype::server {

/**
 * @param results Keeps what each game leaves; it must outlive the modules.
 * @returns The modules of a game of R-Type, in the order a game installs them:
 * the replication, which provides what the rules require, the rules, then the
 * results, which read how the game stands right after the rules ran. A game of
 * another game lists its own.
 */
[[nodiscard]] std::vector<std::unique_ptr<IGameModule>> rtypeGameModules(
    IResultsArchive& results);

}  // namespace rtype::server
