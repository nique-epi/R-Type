#pragma once

namespace rtype::game::testing {

/**
 * @brief Where the world tests spawn their entities, in logical units: some
 * place, and another one far enough to tell two entities apart.
 */
constexpr float SOME_X = 3.0F;
constexpr float SOME_Y = 4.0F;
constexpr float OTHER_X = 30.0F;
constexpr float OTHER_Y = 40.0F;

/** @brief The health a new Bydo has, as the world tests expect it. */
constexpr int THREE_HEALTH_POINTS = 3;

}  // namespace rtype::game::testing
