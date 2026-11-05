#pragma once

#include <cstddef>
#include <vector>
#include "ActionSet.hpp"
#include "BotPilot.hpp"

namespace rtype::tools::testing {

/** @returns What @p pilot holds at each of @p frames frames of
 * SIMULATION_TICK_DURATION, in order. */
[[nodiscard]] std::vector<client::ActionSet> heldOver(BotPilot& pilot,
                                                      std::size_t frames);

}  // namespace rtype::tools::testing
