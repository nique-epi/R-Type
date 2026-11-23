#pragma once

#include <cstddef>
#include <vector>
#include "EntitySpawned.hpp"
#include "WavePlan.hpp"
#include "WaveSpawner.hpp"
#include "WaveSpawnerScene.hpp"

namespace rtype::server::testing {

/** @returns The waves the server tests play, with the shipped crystal carrier
 * every other wave. */
[[nodiscard]] WavePlan wavePlanWithACarrier();

/** @returns The EntitySpawned of the crystal carrier among @p spawns, in
 * order. */
[[nodiscard]] std::vector<network::EntitySpawned> carriersIn(
    const std::vector<network::EntitySpawned>& spawns);

/** @brief Runs @p spawner for @p ticks ticks, one at a time, clearing the
 * sky of @p scene after each so every wave ends as soon as it appeared. */
void playClearingTheSky(WaveSpawnerScene& scene, WaveSpawner& spawner,
                        std::size_t ticks);

}  // namespace rtype::server::testing
