#include <cstdint>
#include <memory>
#include "IModule.hpp"
#include "PluginConstants.hpp"
#include "PluginEntry.hpp"
#include "ServerGameStatisticsModule.hpp"

std::uint32_t rtypePluginEntry(
    std::unique_ptr<rtype::engine::IModule>* module) {
  if (module != nullptr) {
    *module = std::make_unique<rtype::server::ServerGameStatisticsModule>();
  }
  return rtype::engine::PLUGIN_API_VERSION;
}
