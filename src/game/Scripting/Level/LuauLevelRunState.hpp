#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief Where the run of a level stands: Running while a stage is left to
 * run or to end, Cleared once its last stage returned, Failed once a stage
 * failed, for good. */
enum class LuauLevelRunState : std::uint8_t {
  Running,
  Cleared,
  Failed,
};

}  // namespace rtype::game
