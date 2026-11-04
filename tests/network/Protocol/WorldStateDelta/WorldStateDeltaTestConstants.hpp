#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network::testing {

/** @brief One eighth of a unit, the step of a wire coordinate. */
constexpr float ONE_EIGHTH = 0.125F;

/** @brief A move far below an eighth of a unit: it rounds to the same count. */
constexpr float FAR_BELOW_AN_EIGHTH = 0.01F;

/** @brief Entities in the lists the tests compare. */
constexpr std::size_t ENTITIES_IN_A_LIST = 5;

/** @brief The id of the second entity of such a list, which the tests move,
 * and of the fourth, which they remove. */
constexpr std::uint32_t MOVED_ENTITY_ID = 2;
constexpr std::uint32_t VANISHED_ENTITY_ID = 4;

/** @brief The id of an entity such a list lacks. */
constexpr std::uint32_t NEW_ENTITY_ID = 9;

}  // namespace rtype::network::testing
