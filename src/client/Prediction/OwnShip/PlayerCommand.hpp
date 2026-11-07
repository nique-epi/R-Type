#pragma once

#include <cstdint>
#include "ActionSet.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

/** @brief What the player commands during an update: the actions held, and
 * the tick of the server the client draws the other entities at, which every
 * input sent carries. */
struct PlayerCommand {
  ActionSet held{};
  std::uint32_t shownTick{network::NO_SHOWN_TICK};
};

}  // namespace rtype::client
