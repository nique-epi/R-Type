#pragma once

#include <string_view>
#include "BiomeNameConstants.hpp"

namespace rtype::identifiers {

/**
 * @returns Whether @p name may name a biome: MINIMUM_BIOME_NAME_LENGTH to
 * MAXIMUM_BIOME_NAME_LENGTH bytes of BIOME_NAME_CHARACTERS. The game checks
 * the name a level script gives with it, and the protocol the name of each
 * LevelStatus; it is constexpr so this library stays header-only.
 */
constexpr bool isBiomeName(std::string_view name) {
  return name.size() >= MINIMUM_BIOME_NAME_LENGTH &&
         name.size() <= MAXIMUM_BIOME_NAME_LENGTH &&
         name.find_first_not_of(BIOME_NAME_CHARACTERS) ==
             std::string_view::npos;
}

}  // namespace rtype::identifiers
