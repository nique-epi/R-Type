#pragma once

#include <numbers>
#include <string_view>

namespace rtype::server {

/**
 * @brief Logger name the weapon system writes its log lines under.
 */
constexpr std::string_view WEAPON_SYSTEM_LOGGER_NAME = "WeaponSystem";

/** @brief The direction of the first shard of a burst, up and to the right,
 * in radians from the right; the others follow it, spread evenly over the
 * whole turn (FULL_TURN_RADIANS): four shards fly along the four diagonals. */
constexpr float FIRST_SHARD_ANGLE_RADIANS = std::numbers::pi_v<float> / 4.0F;

/** @brief How far from the center of its projectile a shard appears, in
 * logical units: far enough for the scatter behaviour to know its way, near
 * enough to look like it came out of it. */
constexpr float SHARD_START_DISTANCE = 8.0F;

}  // namespace rtype::server
