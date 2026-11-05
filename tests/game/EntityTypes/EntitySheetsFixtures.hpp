#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "EntitySheets.hpp"
#include "SheetText.hpp"

namespace rtype::game::testing {

/** @returns The sheet @p name whose file holds @p text, the errors naming it
 * `<name>.toml`. */
[[nodiscard]] SheetText sheetText(std::string_view name, std::string_view text);

/** @returns @p count sheets of a missile of an enemy, named `missile_0` and
 * on. */
[[nodiscard]] std::vector<SheetText> enemyMissileSheets(std::size_t count);

/** @returns The names of the sheets of @p sheets, in their order. */
[[nodiscard]] std::vector<std::string> namesIn(const EntitySheets& sheets);

}  // namespace rtype::game::testing
