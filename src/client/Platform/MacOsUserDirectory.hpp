#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief A folder of the user's Library that macOS names: the Library itself,
 * or its Application Support folder. */
enum class MacOsUserDirectory : std::uint8_t { Library, ApplicationSupport };

}  // namespace rtype::client
