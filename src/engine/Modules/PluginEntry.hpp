#pragma once

#include <cstdint>
#include <memory>
#include "IModule.hpp"

/**
 * @brief The one function a plugin exports, with C linkage so its name stays
 * PLUGIN_ENTRY_NAME whatever the compiler. A plugin defines it once.
 *
 * Called with nullptr, it creates nothing and returns the plugin API version
 * the plugin was built against. Called with a slot, it creates the module of
 * the plugin into it and returns that version again. The program asks for the
 * version first, and creates the module only when it equals
 * PLUGIN_API_VERSION: the layout of IModule may differ between two versions.
 */
extern "C" std::uint32_t rtypePluginEntry(
    std::unique_ptr<rtype::engine::IModule>* module);

namespace rtype::engine {

/** @brief The type of rtypePluginEntry, as the loader finds it by name. */
using PluginEntry = decltype(&rtypePluginEntry);

}  // namespace rtype::engine
