#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Points earned for destroying a Drifter.
 *
 * Provisional: gameplay will tune it.
 */
constexpr std::uint32_t DRIFTER_POINTS = 100;

/**
 * @brief Points earned for destroying a Weaver, which is harder to hit than a
 * Drifter.
 *
 * Provisional: gameplay will tune it.
 */
constexpr std::uint32_t WEAVER_POINTS = 200;

}  // namespace rtype::game
