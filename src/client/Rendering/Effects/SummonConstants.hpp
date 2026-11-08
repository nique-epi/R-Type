#pragma once

#include <array>
#include <chrono>
#include "BossArrivalConstants.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "Ring.hpp"
#include "SummoningConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief An enemy that appears inside the playfield was summoned by a
 * volley: the waves enter from past its right edge. The server keeps it
 * harmless for SUMMON_WARNING_TIME (game::SUMMONING_TIME); the client shows it
 * hidden then drawn row by row over that time (see Reveal), behind SUMMON_RINGS
 * rings opened SUMMON_RING_PERIOD apart where it appears: two beats a second,
 * under the three flashes a second accessibility guidelines allow. */
constexpr engine::Duration SUMMON_WARNING_TIME = game::SUMMONING_TIME;
constexpr engine::Duration SUMMON_RING_PERIOD = std::chrono::milliseconds(500);
constexpr int SUMMON_RINGS = 2;
constexpr Ring SUMMON_RING{.radius = 24, .life = SUMMON_RING_PERIOD};
constexpr auto SUMMON_RING_RAMP = ROAR_BONE_RAMP;

/** @returns Whether an enemy appearing at @p center was summoned: it stands
 * inside the playfield. */
constexpr bool isSummonedAt(game::Position center) {
  return center.x < game::PLAYFIELD_WIDTH;
}

}  // namespace rtype::client
