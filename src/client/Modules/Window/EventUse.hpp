#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What a listener of the window events does with an event it was
 * shown. */
enum class EventUse : std::uint8_t {
  /** @brief The event goes on to the listener added before this one. */
  Passed,
  /** @brief The event stops here: no listener added before this one sees
   * it. */
  Kept,
};

}  // namespace rtype::client
