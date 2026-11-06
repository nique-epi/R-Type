#pragma once

#include <string_view>

namespace rtype::master {

/** @brief The names the modules of the master give to depend on each other. */
constexpr std::string_view LOGGER_MODULE_NAME = "logger";
constexpr std::string_view STORAGE_MODULE_NAME = "storage";
constexpr std::string_view HTTP_MODULE_NAME = "http";
constexpr std::string_view SERVERS_MODULE_NAME = "servers";

}  // namespace rtype::master
