#pragma once

#include <cstdint>
#include "Endpoint.hpp"

namespace rtype::server {

/** @brief The list of players whose datagrams the network thread lets through
 * to the tick. */
class IPlayerAdmission {
 public:
  virtual ~IPlayerAdmission() = default;

  /**
   * @brief From now on, the datagrams @p player sends with @p sessionToken
   * reach the tick, and those it sends with another token are dropped. Call it
   * from any thread, once the player holds a place; it takes effect on the
   * network thread shortly after, and a datagram the player sends before that
   * is dropped like a stranger's.
   */
  virtual void admit(const network::Endpoint& player,
                     std::uint64_t sessionToken) = 0;

  /**
   * @brief From now on, the datagrams @p player sends are dropped like a
   * stranger's, unless it was admitted again with another token than
   * @p sessionToken. Call it from any thread once the session of the player
   * closed; it takes effect on the network thread shortly after.
   */
  virtual void forget(const network::Endpoint& player,
                      std::uint64_t sessionToken) = 0;
};

}  // namespace rtype::server
