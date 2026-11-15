#pragma once

#include <string>
#include <string_view>
#include "LdtkPlacement.hpp"
#include "Position.hpp"

/** @returns A placed enemy of the sheet @p sheet, its entity named after the
 * sheet, its center at @p center and @p width units wide, without fields. */
[[nodiscard]] rtype::game::LdtkPlacement placedEnemyOf(
    std::string_view sheet, rtype::game::Position center, float width);

/** @returns The text of an LDtk project of one level, Level_1, of 480 by 270
 * pixels, whose Mobs layer of entities holds @p entityInstances, the JSON of
 * its entities separated by commas. */
[[nodiscard]] std::string oneLevelProject(std::string_view entityInstances);
