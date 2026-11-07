#pragma once

#include <optional>
#include "BehaviourView.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @returns The view of an idle, calm Tyrant of the shipped settings at
 * @p position, which appeared at WHERE_THE_TYRANT_APPEARS long ago, its
 * arrival over and rested, a tick long, with @p ship nearest. */
[[nodiscard]] BehaviourView tyrantView(Position position,
                                       std::optional<Position> ship);

}  // namespace rtype::game::testing
