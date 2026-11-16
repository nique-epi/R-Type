#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the player answered on the sales slip of a ware, if
 * anything. */
enum class TicketAnswer : std::uint8_t {
  None,
  Buy,
  Cancel,
};

}  // namespace rtype::client
