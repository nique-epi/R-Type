#pragma once

#include <cstdint>
#include "ComponentRegistry.hpp"

namespace rtype::game::testing {

/**
 * @brief Renders one second at @p framesPerSecond, running a MovementSystem on
 * @p components once per tick FixedTimestep returns. Frame lengths are whole
 * nanoseconds that add up to exactly one second.
 */
void runOneSecondAt(std::int64_t framesPerSecond,
                    engine::ComponentRegistry& components);

}  // namespace rtype::game::testing
