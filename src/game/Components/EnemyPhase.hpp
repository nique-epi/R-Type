#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief Where an attack of an enemy stands; see network::EnemyActionPhase. */
enum class EnemyPhase : std::uint8_t {
  Signal,
  Strike,
  Recover,
  Enraged,
};

}  // namespace rtype::game
