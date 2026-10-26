#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief Module name the spawner writes its log lines under.
 */
constexpr std::string_view BYDO_SPAWNER_LOGGER_NAME = "BydoSpawner";

}  // namespace rtype::server
