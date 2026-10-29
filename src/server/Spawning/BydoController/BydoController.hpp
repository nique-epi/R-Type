#pragma once

#include "World.hpp"

namespace rtype::server {

/**
 * @brief Writes the Intent of every Bydo of @p world: the artificial
 * intelligence of the Bydos, which wants to shoot as long as it is alive.
 *
 * This is the artificial intelligence side of the control of a body: how often
 * a Bydo fires is the interval of its Gun, and how it moves is its Velocity and
 * its MovementPattern (see PatternSystem); the weapon system and MovementSystem
 * never ask which controller wrote the Intent.
 */
void steerBydos(game::World& world);

}  // namespace rtype::server
