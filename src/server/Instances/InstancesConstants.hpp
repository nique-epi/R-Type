#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief Module name the classes that hold the players of the game write their
 * log lines under.
 */
constexpr std::string_view INSTANCES_LOGGER_NAME = "Instances";

}  // namespace rtype::server
