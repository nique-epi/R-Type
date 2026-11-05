#pragma once

#include <cstdint>
#include <string_view>
#include "PluginConstants.hpp"

/** @brief The name of the module a test plugin depends on. */
constexpr std::string_view PLUGIN_HOST_MODULE_NAME = "host";

/** @brief The name of the module the probe plugin creates. */
constexpr std::string_view PROBE_MODULE_NAME = "probe";

/** @brief The plugin API version the future plugin claims: one the program does
 * not know yet. */
constexpr std::uint32_t FUTURE_PLUGIN_API_VERSION =
    rtype::engine::PLUGIN_API_VERSION + 1;
