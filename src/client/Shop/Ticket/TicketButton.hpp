#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The two buttons of the sales slip of a ware. */
enum class TicketButton : std::uint8_t {
  Cancel,
  Buy,
};

}  // namespace rtype::client
