#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::server {

/**
 * @brief Logger name the spawner writes its log lines under.
 */
constexpr std::string_view WAVE_SPAWNER_LOGGER_NAME = "WaveSpawner";

/** @brief The first wave that may bring a look item. */
constexpr std::size_t SECOND_WAVE = 2;

/**
 * @brief Where a look item enters, from the top of the right edge: the middle.
 * It is not drawn at random, so the enemies of a seed keep their places.
 */
constexpr float LOOK_ITEM_ENTRY_HEIGHT = 0.5F;

/**
 * @brief Where a crystal carrier enters, from the top of the right edge: a
 * quarter of the way down, above a look item that comes with the same wave.
 */
constexpr float CARRIER_ENTRY_HEIGHT = 0.25F;

}  // namespace rtype::server
