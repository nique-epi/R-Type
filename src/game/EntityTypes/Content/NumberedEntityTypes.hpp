#pragma once

#include <span>
#include <vector>
#include "NumberedEntityType.hpp"
#include "SheetText.hpp"

namespace rtype::game {

/**
 * @returns The types @p sheets describe, numbered as EntitySheets numbers
 * them: from 1, in the order of their names. Of `[gameplay]` it reads the side,
 * the box (hitboxOf()) and the contact of an obstacle (obstacleContactOf())
 * only, so a client reads the sheets of a server whose behaviours it does not
 * know.
 * @throws What checkSheetNames() throws; engine::DataFileSyntaxException,
 * MissingDataFileKeyException, DataFileTableExpectedException or
 * UnknownDataFileChoiceException when a sheet holds no side of SIDE_NAMES;
 * and what hitboxOf() throws for its box, and obstacleContactOf() for its
 * obstacle table.
 */
[[nodiscard]] std::vector<NumberedEntityType> numberedEntityTypesOf(
    std::span<const SheetText> sheets);

}  // namespace rtype::game
