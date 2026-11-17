#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "TicketReading.hpp"

namespace rtype::client {

/**
 * @brief Asks the master a game ticket for the account of a session, without
 * the frame ever waiting on HTTP: an ask returns its number at once, and its
 * reading shows up on a later frame. A newer ask replaces the one before it,
 * unserved. The token and the tickets are kept in memory only: never logged,
 * never written.
 */
class ITicketLink {
 public:
  virtual ~ITicketLink() = default;

  /** @returns The number of this ask for a ticket of the account of
   * @p token. */
  virtual std::uint64_t askTicket(const std::string& token) = 0;

  /** @returns The newest answer, nothing before the first. */
  [[nodiscard]] virtual std::optional<TicketReading> ticket() const = 0;
};

}  // namespace rtype::client
