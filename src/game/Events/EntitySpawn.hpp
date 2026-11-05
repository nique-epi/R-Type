#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief An entity appeared: its network id, the number of its sheet (see
 * EntityTypeNumber) and where its center is. The network id is the one of its
 * NetworkIdentity, read when the event is published.
 */
struct EntitySpawn {
  std::uint32_t networkId{};
  std::uint8_t typeNumber{};
  float x{};
  float y{};

  bool operator==(const EntitySpawn&) const = default;
};

}  // namespace rtype::game
