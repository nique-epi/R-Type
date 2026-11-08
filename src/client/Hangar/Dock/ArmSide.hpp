#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Whether the base of an arm hangs from the ceiling of the bay or
 * stands on its floor. */
enum class ArmSide : std::uint8_t {
  Ceiling,
  Floor,
};

}  // namespace rtype::client
