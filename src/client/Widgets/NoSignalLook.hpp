#pragma once

#include "BarState.hpp"
#include "NoSignalReason.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief What the NO SIGNAL card shows at one moment. */
struct NoSignalLook {
  NoSignalReason reason{};
  /** @brief Time since the card showed. */
  engine::Duration sinceCard{};
  /** @brief The call to reconnect: at rest, under the pointer, or pressed. */
  BarState call = BarState::Idle;
  /** @brief Whether the call never blinks: motion or flashes reduced. */
  bool steady = false;
};

}  // namespace rtype::client
