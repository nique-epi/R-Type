#pragma once

#include <cstdint>
#include "World.hpp"

namespace rtype::server::testing {

/** @brief Gives the enemy of network id @p networkId the marker
 * SurvivesContact. */
void markAsSurvivor(game::World& world, std::uint32_t networkId);

/** @returns Whether an entity of network id @p networkId is in @p world. */
[[nodiscard]] bool isInTheWorld(game::World& world, std::uint32_t networkId);

}  // namespace rtype::server::testing
