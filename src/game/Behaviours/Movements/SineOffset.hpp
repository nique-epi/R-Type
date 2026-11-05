#pragma once

#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @returns How far from its row a body that weaves or bobs is at @p age:
 * @p amplitude × sin(2π × @p age ÷ @p period), in the unit of @p amplitude. It
 * is computed from the age, never added up tick by tick, so the path does not
 * depend on how the simulation time is cut into steps.
 */
[[nodiscard]] float sineOffset(engine::Duration age, float amplitude,
                               engine::Duration period);

}  // namespace rtype::game
