#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief The network identifier the server gave to an entity, which clients
 * use to name it. It must be set to the identifier a NetworkIdAllocator gave:
 * a value-initialized one (NetworkIdentity{}) holds NO_ENTITY_ID, which no
 * entity has.
 */
struct NetworkIdentity {
  std::uint32_t networkId;
};

}  // namespace rtype::game
