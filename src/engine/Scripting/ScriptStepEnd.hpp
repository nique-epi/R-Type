#pragma once

namespace rtype::engine {

/** @brief How a step of a script thread ended: its function returned, or it
 * yielded and waits to be resumed. */
enum class ScriptStepEnd { Finished, Yielded };

}  // namespace rtype::engine
