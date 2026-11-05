#pragma once

#include <optional>
#include "BehaviourView.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @returns The view of an idle Reaper of the shipped settings at
 * @p position, rested, a tick long, with @p ship nearest. */
[[nodiscard]] BehaviourView reaperView(Position position,
                                       std::optional<Position> ship);

}  // namespace rtype::game::testing
