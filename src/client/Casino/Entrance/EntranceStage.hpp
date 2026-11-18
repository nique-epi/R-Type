#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where the player stands at the casino. */
enum class EntranceStage : std::uint8_t {
  /** @brief Before the doors, closed: ENTER or BACK. */
  Doors,
  /** @brief The doors open while the butler greets and bows; any key or a
   * click skips it. */
  Opening,
  /** @brief Inside, before the menu of the hall. */
  Hall,
};

}  // namespace rtype::client
