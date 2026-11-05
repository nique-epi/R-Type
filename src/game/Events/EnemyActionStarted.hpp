#pragma once

#include <cstdint>
#include "EnemyPhase.hpp"

namespace rtype::game {

/** @brief Event published when an enemy enters a phase of an attack. The
 * network id is the one of its NetworkIdentity. */
struct EnemyActionStarted {
  std::uint32_t networkId;
  std::uint8_t attack;
  EnemyPhase phase;

  bool operator==(const EnemyActionStarted&) const = default;
};

}  // namespace rtype::game
