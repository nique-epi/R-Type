#pragma once

#include <cstdint>

namespace rtype::engine {

/** @brief How a step of a script thread ended: its function returned, or it
 * yielded and waits to be resumed. */
enum class ScriptStepEnd : std::uint8_t { Finished, Yielded };

}  // namespace rtype::engine
