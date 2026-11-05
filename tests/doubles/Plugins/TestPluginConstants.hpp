#pragma once

#include <cstdint>
#include <string_view>
#include "PluginConstants.hpp"

/** @brief The name of the module a test plugin depends on. */
constexpr std::string_view PLUGIN_HOST_MODULE_NAME = "host";

/** @brief The name of the module the probe plugin creates. */
constexpr std::string_view PROBE_MODULE_NAME = "probe";

/** @brief The name of the module the greeter plugin creates. */
constexpr std::string_view GREETER_MODULE_NAME = "greeter";

/** @brief The name of the module the greeter plugin depends on: the logger
 * module of the server, which provides the journal. */
constexpr std::string_view LOGGER_MODULE_NAME_IN_TESTS = "logger";

/** @brief The name of the logger the greeter plugin writes with. */
constexpr std::string_view GREETER_LOGGER_NAME = "Greeter";

/** @brief The lines the greeter plugin writes when it starts and stops. */
constexpr std::string_view GREETER_STARTED_TEXT = "greeter started";
constexpr std::string_view GREETER_STOPPED_TEXT = "greeter stopped";

/** @brief The plugin API version the future plugin claims: one the program does
 * not know yet. */
constexpr std::uint32_t FUTURE_PLUGIN_API_VERSION =
    rtype::engine::PLUGIN_API_VERSION + 1;
