#pragma once

#include <chrono>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Time between two Bydos appearing, in simulation time.
 *
 * Provisional: one Bydo every two seconds, until the waves of rising
 * difficulty set the pace.
 */
constexpr engine::Duration BYDO_SPAWN_INTERVAL = std::chrono::seconds(2);

/**
 * @brief Module name the spawner writes its log lines under.
 */
constexpr std::string_view BYDO_SPAWNER_LOGGER_NAME = "BydoSpawner";

}  // namespace rtype::server
