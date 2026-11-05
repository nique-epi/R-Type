#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::engine {

/**
 * @brief The version of everything a plugin compiles against: IModule,
 * InstallContext, ServiceRegistry, LoopStages, EventBus, Logger, ILogJournal,
 * PluginEntry, and ConfigurationRegistry with the definitions of the settings
 * it declares. Raised by every change to one of those headers. A plugin is also
 * built by the same compiler, from the same sources, as the program that loads
 * it: the C++ binary format changes from one compiler to another.
 */
constexpr std::uint32_t PLUGIN_API_VERSION = 1;

/** @brief The exported name of rtypePluginEntry. */
constexpr std::string_view PLUGIN_ENTRY_NAME = "rtypePluginEntry";

/** @brief The command that loads a plugin. */
constexpr std::string_view PLUGIN_COMMAND_NAME = "plugin";

/** @brief The command that sets the folder the plugin lines read. */
constexpr std::string_view PLUGIN_FOLDER_COMMAND_NAME = "plugin-folder";

/** @brief The folder, next to the executable, the plugin lines read by
 * default. */
constexpr std::string_view DEFAULT_PLUGIN_FOLDER_NAME = "plugins";

}  // namespace rtype::engine
