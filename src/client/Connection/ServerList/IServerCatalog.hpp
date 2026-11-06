#pragma once

#include "ServerListSnapshot.hpp"

namespace rtype::client {

/**
 * @brief The list of the servers a player may join, as the server map reads
 * it: those the master lists online or degraded, each with the ping the client
 * measured, those that did not answer left out. Neither call makes the frame
 * wait.
 */
class IServerCatalog {
 public:
  virtual ~IServerCatalog() = default;

  /** @brief Reads the list again: the master, then the pings. The snapshot
   * keeps the list before until the new one is complete. */
  virtual void refresh() = 0;

  /** @returns How the list stands. Valid until the next update of the
   * connection module. */
  [[nodiscard]] virtual const ServerListSnapshot& snapshot() const = 0;
};

}  // namespace rtype::client
