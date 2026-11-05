#pragma once

#include "BehaviourView.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @returns The view of an idle Drillback of the shipped settings at
 * @p position, rested, a tick long. */
[[nodiscard]] BehaviourView drillbackView(Position position);

}  // namespace rtype::game::testing
