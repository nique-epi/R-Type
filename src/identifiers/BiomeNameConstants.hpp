#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::identifiers {

/** @brief The sizes the name of a biome may have, in bytes. */
constexpr std::size_t MINIMUM_BIOME_NAME_LENGTH = 1;
constexpr std::size_t MAXIMUM_BIOME_NAME_LENGTH = 24;

/** @brief The bytes a biome name is made of: lowercase letters, digits and
 * underscores, so the name of its file is the same on every system. */
constexpr std::string_view BIOME_NAME_CHARACTERS =
    "abcdefghijklmnopqrstuvwxyz0123456789_";

}  // namespace rtype::identifiers
