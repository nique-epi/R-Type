#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "EntitySheet.hpp"

namespace rtype::game::testing {

/** @returns The name of the behaviour that pilots @p sheet, empty for a body
 * a player pilots. */
[[nodiscard]] std::string_view pilotBehaviourName(const EntitySheet& sheet);

/** @returns The values of the settings of the pilot of @p sheet, none for a
 * body a player pilots. */
[[nodiscard]] std::vector<std::int64_t> settingsOf(const EntitySheet& sheet);

/** @returns The ticks between two shots of the gun of @p sheet, 0 without a
 * gun. */
[[nodiscard]] std::size_t intervalTicksOf(const EntitySheet& sheet);

/** @returns The ticks before the first shot of the gun of @p sheet, 0 without
 * a gun. */
[[nodiscard]] std::size_t firstShotTicksOf(const EntitySheet& sheet);

/** @returns The name of the sheet the gun of @p sheet fires, empty without a
 * gun. */
[[nodiscard]] std::string projectileNameOf(const EntitySheet& sheet);

/** @returns The names of the collision layers @p sheet hits, in the order of
 * its file. */
[[nodiscard]] std::vector<std::string> hitLayersOf(const EntitySheet& sheet);

}  // namespace rtype::game::testing
