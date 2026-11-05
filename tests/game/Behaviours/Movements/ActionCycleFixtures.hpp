#pragma once

#include <optional>
#include "ActionKey.hpp"
#include "BehaviourView.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @returns The view of a body in @p action (idle without one) for @p age. */
[[nodiscard]] BehaviourView viewIn(std::optional<ActionKey> action,
                                   engine::Duration age);

}  // namespace rtype::game::testing
