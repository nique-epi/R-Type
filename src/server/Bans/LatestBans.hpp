#pragma once

#include "BanList.hpp"
#include "LatestValue.hpp"

namespace rtype::server {

/** @brief The bans in force a game process read last from the file of the
 * server: BanWatch publishes them, the lobby of the game reads them. */
using LatestBans = concurrency::LatestValue<BanList>;

}  // namespace rtype::server
