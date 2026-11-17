#pragma once

#include <cstdint>
#include <optional>
#include "GameTicket.hpp"

namespace rtype::client {

/** @brief What the master answered an ask for a game ticket: the number of
 * the ask, and the ticket; none when the master refused, did not answer, or
 * answered what cannot be read. */
struct TicketReading {
  std::uint64_t ask{};
  std::optional<network::GameTicket> ticket = std::nullopt;
};

}  // namespace rtype::client
