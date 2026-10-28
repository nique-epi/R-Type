#pragma once

#include <functional>
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief What a module runs in a loop stage, once per pass of the loop, with
 * the time that pass simulates or draws.
 *
 * It holds the objects it works on from the module's install(), so it looks
 * nothing up while it runs.
 */
using StageWork = std::function<void(Duration elapsed)>;

}  // namespace rtype::engine
