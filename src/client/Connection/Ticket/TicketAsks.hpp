#pragma once

#include <cstdint>
#include <string>

namespace rtype::client {

/** @brief The newest ask for a game ticket, as MasterTicketLink hands it to
 * its thread: its number, one above the previous one, 0 before the first, and
 * the token of the session it asks with, kept in memory only. */
struct TicketAsks {
  std::uint64_t ask{};
  std::string token;
};

}  // namespace rtype::client
