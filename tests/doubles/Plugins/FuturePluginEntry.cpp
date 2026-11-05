#include <cstdint>
#include <memory>
#include "IModule.hpp"
#include "PluginEntry.hpp"
#include "TestPluginConstants.hpp"

std::uint32_t rtypePluginEntry(
    [[maybe_unused]] std::unique_ptr<rtype::engine::IModule>* module) {
  return FUTURE_PLUGIN_API_VERSION;
}
