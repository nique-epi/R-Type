#pragma once

#include <string_view>
#include "ContentNameConstants.hpp"

namespace rtype::identifiers {

/**
 * @returns Whether @p name may name a piece of the content (an entity type,
 * an effect or a look of its appearance): MINIMUM_CONTENT_NAME_LENGTH to
 * MAXIMUM_CONTENT_NAME_LENGTH bytes of CONTENT_NAME_CHARACTERS. The game checks
 * the names of each sheet with it, and the protocol each name it carries; it is
 * constexpr so this library stays header-only.
 */
constexpr bool isContentName(std::string_view name) {
  return name.size() >= MINIMUM_CONTENT_NAME_LENGTH &&
         name.size() <= MAXIMUM_CONTENT_NAME_LENGTH &&
         name.find_first_not_of(CONTENT_NAME_CHARACTERS) ==
             std::string_view::npos;
}

}  // namespace rtype::identifiers
