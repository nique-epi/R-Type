#pragma once

#include <cstdint>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"

namespace rtype::game::testing {

/**
 * @brief Renders one second at @p framesPerSecond, running a MovementSystem on
 * @p components once per tick FixedTimestep returns. Frame lengths are whole
 * nanoseconds that add up to exactly one second.
 */
void runOneSecondAt(std::int64_t framesPerSecond,
                    engine::ComponentRegistry& components);

/**
 * @returns A new entity of @p components at (START_X, START_Y), a steady body
 * that goes at (SPEED_X, SPEED_Y) units per second: a top speed of SPEED_X
 * and an Intent of a full share to the right and SPEED_Y / SPEED_X down.
 */
[[nodiscard]] engine::Entity steadyBodyAtSpeed(
    engine::EntityRegistry& entities, engine::ComponentRegistry& components);

}  // namespace rtype::game::testing
