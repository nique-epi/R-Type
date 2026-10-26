#pragma once

#include <vector>
#include "ConnectedPlayer.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"

namespace rtype::server {

/**
 * @brief A datagram of a player of a game that passed every check of
 * GameInstance::onTick(), with what reading it needs.
 */
struct PlayerDatagram {
  ConnectedPlayer* player{};
  network::MessageOrigin origin;
  std::vector<network::MessageFrame> messages;
};

}  // namespace rtype::server
