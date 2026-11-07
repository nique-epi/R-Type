#pragma once

#include <cstdint>
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief The newest input applied to the ship of a player: its inputNumber,
 * which the world state confirms to its client, and the tick its client
 * showed when it sent it (PlayerInput::shownTick), against whose enemies the
 * missiles of the player are tested (see giveShooterViews()).
 */
struct AppliedInput {
  std::uint32_t inputNumber{0};
  std::uint32_t shownTick{network::NO_SHOWN_TICK};
};

}  // namespace rtype::server
