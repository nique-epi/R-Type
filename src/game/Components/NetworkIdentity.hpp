#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The network identifier the server gave to an entity, which clients
 * use to name it. Never NO_NETWORK_ID.
 */
struct NetworkIdentity {
  std::uint32_t networkId;
};

}  // namespace rtype::game
