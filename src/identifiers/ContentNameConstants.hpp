#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::identifiers {

/** @brief The sizes a name of the content may have, in bytes: the name of an
 * entity type, and the name of an effect or a look of its appearance. */
constexpr std::size_t MINIMUM_CONTENT_NAME_LENGTH = 1;
constexpr std::size_t MAXIMUM_CONTENT_NAME_LENGTH = 24;

/** @brief The bytes a name of the content is made of: lowercase letters,
 * digits and underscores, so the name of a file is the same on every system. */
constexpr std::string_view CONTENT_NAME_CHARACTERS =
    "abcdefghijklmnopqrstuvwxyz0123456789_";

}  // namespace rtype::identifiers
