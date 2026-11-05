#include "ApplicationPluginsFixtures.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "IModule.hpp"
#include "PluginConstants.hpp"
#include "PluginEntry.hpp"
#include "PluginFiles.hpp"
#include "PluginHostModule.hpp"
#include "PluginHostState.hpp"
#include "SharedLibrary.hpp"
#include "TestPluginConstants.hpp"

HandLoadedPlugin::HandLoadedPlugin(const std::string& name)
    : library_(pluginFile(name)) {
  void* const address =
      library_.symbol(std::string{rtype::engine::PLUGIN_ENTRY_NAME});
  // The system returns an object address for a function the library exports.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto entry = reinterpret_cast<rtype::engine::PluginEntry>(address);
  if (entry(nullptr) == rtype::engine::PLUGIN_API_VERSION) {
    static_cast<void>(entry(&module_));
  }
}

std::unique_ptr<rtype::engine::IModule> HandLoadedPlugin::takeModule() {
  return std::move(module_);
}

std::vector<std::unique_ptr<rtype::engine::IModule>> hostThenPlugin(
    PluginHostState& state, HandLoadedPlugin& plugin) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<PluginHostModule>(
      std::string{PLUGIN_HOST_MODULE_NAME}, state));
  modules.push_back(plugin.takeModule());
  return modules;
}
