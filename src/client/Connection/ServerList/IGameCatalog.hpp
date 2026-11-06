#pragma once

#include "GameListSnapshot.hpp"
#include "ServerRow.hpp"

namespace rtype::client {

/** @brief The games of one server, as the panel of the server map reads them.
 * Neither call makes the frame wait. */
class IGameCatalog {
 public:
  virtual ~IGameCatalog() = default;

  /** @brief Reads the games of @p server, in place of the read under way: the
   * snapshot goes back to Searching until they come. */
  virtual void ask(const ServerRow& server) = 0;

  /** @returns How the read stands. Valid until the next update of the
   * connection module. */
  [[nodiscard]] virtual const GameListSnapshot& snapshot() const = 0;
};

}  // namespace rtype::client
