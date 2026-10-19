#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Why an entity disappears: Killed shows an explosion, Removed shows
 * nothing, such as a missile leaving the playfield.
 */
enum class DestructionCause : std::uint8_t {
  Killed,
  Removed,
};

/**
 * @brief Event published when an entity is destroyed. The network id is the
 * one of its NetworkIdentity, read before the entity is removed.
 */
struct EntityDestruction {
  std::uint32_t networkId;
  DestructionCause cause;

  bool operator==(const EntityDestruction&) const = default;
};

}  // namespace rtype::game
