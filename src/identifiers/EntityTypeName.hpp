#pragma once

#include <string_view>
#include "EntityTypeConstants.hpp"

namespace rtype::identifiers {

/**
 * @returns Whether @p name may name an entity type:
 * MINIMUM_ENTITY_TYPE_NAME_LENGTH to MAXIMUM_ENTITY_TYPE_NAME_LENGTH bytes of
 * ENTITY_TYPE_NAME_CHARACTERS. The game checks the name of each sheet with it,
 * and the protocol each name of ConnectionAccepted; it is constexpr so this
 * library stays header-only.
 */
constexpr bool isEntityTypeName(std::string_view name) {
  return name.size() >= MINIMUM_ENTITY_TYPE_NAME_LENGTH &&
         name.size() <= MAXIMUM_ENTITY_TYPE_NAME_LENGTH &&
         name.find_first_not_of(ENTITY_TYPE_NAME_CHARACTERS) ==
             std::string_view::npos;
}

}  // namespace rtype::identifiers
