#pragma once

#include <string_view>

namespace rtype::client {

/** @brief The names the modules of the client give to depend on each other,
 * in the order clientModules() lists them. */
constexpr std::string_view LOGGER_MODULE_NAME = "logger";
constexpr std::string_view ASSETS_MODULE_NAME = "assets";
constexpr std::string_view SETTINGS_MODULE_NAME = "settings";
constexpr std::string_view WINDOW_MODULE_NAME = "window";
constexpr std::string_view CONNECTION_MODULE_NAME = "connection";
constexpr std::string_view SCREENS_MODULE_NAME = "screens";

}  // namespace rtype::client
