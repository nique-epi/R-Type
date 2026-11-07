#pragma once

#include "Gun.hpp"

namespace rtype::game {

/**
 * @brief Counts one tick off @p gun, when it has some left: the count down the
 * server runs on every gun, and the client on the gun it predicts.
 * @returns Whether @p gun is ready to fire after that tick.
 */
constexpr bool countsDownToReady(Gun& gun) {
  if (gun.ticksUntilReady > 0) {
    --gun.ticksUntilReady;
  }
  return gun.ticksUntilReady == 0;
}

}  // namespace rtype::game
