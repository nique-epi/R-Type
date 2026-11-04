#pragma once

#include <span>
#include <vector>
#include "StateDelta.hpp"
#include "WorldState.hpp"

namespace rtype::network {

/**
 * @brief Compares two lists of entities, each sorted by entityId with no id
 * twice.
 * @returns The difference from @p base to @p current. An entity is changed
 * when @p base lacks it, when its type differs, or when x or y, rounded to the
 * wire (an eighth of a unit), differ: a move smaller than that is not
 * changed, since the receiver could not see it. Both lists of the result are
 * sorted by id.
 * @throws InvalidCoordinateException when a coordinate is NaN.
 */
[[nodiscard]] StateDelta diffStates(std::span<const EntityState> base,
                                    std::span<const EntityState> current);

/**
 * @brief Applies @p delta to @p base, the inverse of diffStates().
 * @returns The entities of @p base that @p delta does not remove or replace,
 * and those it changes, sorted by entityId. @p base must be sorted by
 * entityId; the lists of @p delta may come in any order, and an id listed
 * twice in `changed` keeps its first entry, and an entity of `changed` replaces
 * the one of @p base with its id.
 */
[[nodiscard]] std::vector<EntityState> applyDelta(
    std::span<const EntityState> base, const StateDelta& delta);

}  // namespace rtype::network
