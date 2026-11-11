#pragma once

#include <cstddef>
#include <cstdint>
#include "CollisionFilter.hpp"

/** @brief The index of the first layer and of the last a filter tells apart,
 * and the set each one makes: the lowest and the highest bit of 32. */
constexpr std::size_t FIRST_LAYER_INDEX = 0;
constexpr std::size_t LAST_LAYER_INDEX = 31;
constexpr std::uint32_t FIRST_LAYER_SET = 0x00000001U;
constexpr std::uint32_t LAST_LAYER_SET = 0x80000000U;

/** @brief Three layers of the tests, one bit each. */
constexpr std::uint32_t SHOT_LAYER = 0x1U;
constexpr std::uint32_t ROCK_LAYER = 0x2U;
constexpr std::uint32_t DUST_LAYER = 0x4U;

/** @brief A shot that hits the rocks, a rock and dust that hit nothing, and
 * an entity that lies nowhere and hits nothing. */
constexpr rtype::engine::CollisionFilter SHOT_HITTING_ROCKS{.layer = SHOT_LAYER,
                                                            .hits = ROCK_LAYER};
constexpr rtype::engine::CollisionFilter ROCK{.layer = ROCK_LAYER, .hits = 0};
constexpr rtype::engine::CollisionFilter DUST{.layer = DUST_LAYER, .hits = 0};
constexpr rtype::engine::CollisionFilter NOWHERE{};
