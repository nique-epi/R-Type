#pragma once

#include "PlayerSettings.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @returns What @p settings let the player buy with and already hold: the
 * kopecks, the chips and the locker. */
[[nodiscard]] Purse purseOf(const PlayerSettings& settings);

}  // namespace rtype::client
