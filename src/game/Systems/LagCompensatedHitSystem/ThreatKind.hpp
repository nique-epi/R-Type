#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief What a box kept by LagCompensatedHitSystem is: the body of an enemy,
 * which a missile of a player hits and a ship runs into; a missile of an
 * enemy, which hits a ship; or an active box of the HurtZone of an enemy,
 * which hurts a ship. */
enum class ThreatKind : std::uint8_t { Body, Missile, HurtBox };

}  // namespace rtype::game
