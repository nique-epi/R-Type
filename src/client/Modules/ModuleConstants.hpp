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
constexpr std::string_view AUDIO_MODULE_NAME = "audio";
constexpr std::string_view SCREEN_REGISTRY_MODULE_NAME = "screen-registry";
constexpr std::string_view MENUS_MODULE_NAME = "menus";
constexpr std::string_view LOBBY_MODULE_NAME = "lobby";
constexpr std::string_view GAME_MODULE_NAME = "game";
constexpr std::string_view SCREENS_MODULE_NAME = "screens";

}  // namespace rtype::client
