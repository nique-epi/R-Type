#pragma once

#include <cstdint>
#include "NetworkIdConstants.hpp"

namespace rtype::engine {

/**
 * @brief Hands out the network identifiers of a game, on the server.
 * An identifier is never given twice, so it stays unique during the game.
 */
class NetworkIdAllocator {
 public:
  NetworkIdAllocator() = default;

  /**
   * @brief Starts after lastAllocated: the next identifier is lastAllocated
   * + 1.
   */
  explicit NetworkIdAllocator(std::uint32_t lastAllocated);

  /**
   * @returns The next identifier: 1 the first time, then one more each time.
   * Never NO_NETWORK_ID.
   * @throws NetworkIdExhaustedException once the identifier 4294967295 was
   * given.
   */
  std::uint32_t allocate();

 private:
  std::uint32_t lastAllocated_{NO_NETWORK_ID};
};

}  // namespace rtype::engine
