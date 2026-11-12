#pragma once

#include <cstdint>
#include <optional>
#include "ActionSet.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

/** @brief What the player commands during an update: the actions held, the
 * tick of the server the client draws the other entities at, which every
 * input sent carries, and the tick the server is at as the command leaves. */
struct PlayerCommand {
  ActionSet held{};
  std::uint32_t shownTick{network::NO_SHOWN_TICK};
  /** @brief Nothing while the client does not know the clock of the server. */
  std::optional<double> serverTick = std::nullopt;
};

}  // namespace rtype::client
