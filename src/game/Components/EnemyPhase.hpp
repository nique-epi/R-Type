#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief Where an attack of an enemy stands. The sheet of an enemy names the
 * phases in lowercase (`signal`, `strike`, `recover`, `enraged`); the wire
 * carries them as network::EnemyActionPhase. */
enum class EnemyPhase : std::uint8_t {
  Signal,
  Strike,
  Recover,
  Enraged,
};

}  // namespace rtype::game
