#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "EntitySheets.hpp"
#include "SheetText.hpp"

namespace rtype::game::testing {

/** @returns @p count sheets of a missile of an enemy, named `missile_0` and
 * on. */
[[nodiscard]] std::vector<SheetText> enemyMissileSheets(std::size_t count);

/** @returns @p count sheets of an enemy, named `layer_0` and on, each in a
 * collision layer named as it is. */
[[nodiscard]] std::vector<SheetText> sheetsOfDistinctLayers(std::size_t count);

/** @returns The names of the sheets of @p sheets, in their order. */
[[nodiscard]] std::vector<std::string> namesIn(const EntitySheets& sheets);

}  // namespace rtype::game::testing
