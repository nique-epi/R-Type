#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief Module name the weapon system writes its log lines under.
 */
constexpr std::string_view WEAPON_SYSTEM_LOGGER_NAME = "WeaponSystem";

}  // namespace rtype::server
