#pragma once

#include "PilotStatus.hpp"

namespace rtype::client {

/**
 * @returns Whether @p pilot has left the game for good: it left, lost its
 * signal or was expelled, so no world state changes its state any more.
 */
[[nodiscard]] bool hasLeft(const PilotStatus& pilot);

}  // namespace rtype::client
