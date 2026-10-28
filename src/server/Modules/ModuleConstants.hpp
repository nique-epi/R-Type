#pragma once

#include <string_view>

namespace rtype::server {

/** @brief The names the modules of the server give to depend on each other. */
constexpr std::string_view JOURNAL_MODULE_NAME = "journal";
constexpr std::string_view NETWORK_MODULE_NAME = "network";
constexpr std::string_view GAMES_MODULE_NAME = "games";
constexpr std::string_view LOBBY_MODULE_NAME = "lobby";

}  // namespace rtype::server
