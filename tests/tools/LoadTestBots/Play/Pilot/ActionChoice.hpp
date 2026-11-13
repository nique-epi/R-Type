#pragma once

#include <cstddef>
#include "ActionSet.hpp"

namespace rtype::tools {

/**
 * @pre @p choice is under ACTION_CHOICE_COUNT.
 * @returns The actions of @p choice: the first half of the choices holds a
 * direction, the second half the same direction and the fire.
 */
[[nodiscard]] client::ActionSet actionSetOf(std::size_t choice);

}  // namespace rtype::tools
