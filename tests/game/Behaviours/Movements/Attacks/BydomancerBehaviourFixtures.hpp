#pragma once

#include <optional>
#include "BehaviourView.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @returns The view of an idle Bydomancer of the shipped settings at
 * @p position, which appeared on the row of AT_ITS_COLUMN, rested, a tick
 * long, with @p ship nearest. */
[[nodiscard]] BehaviourView bydomancerView(Position position,
                                           std::optional<Position> ship);

}  // namespace rtype::game::testing
