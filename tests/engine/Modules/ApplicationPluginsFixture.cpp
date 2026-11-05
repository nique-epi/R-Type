#include "ApplicationPluginsFixture.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ApplicationConfigurationFixtures.hpp"
#include "ApplicationPluginsTestConstants.hpp"
#include "IModule.hpp"
#include "PluginFiles.hpp"
#include "PluginHostModule.hpp"
#include "ProvidingModule.hpp"
#include "TestEvent.hpp"

std::unique_ptr<rtype::engine::IModule> ApplicationPluginsFixture::hostNamed(
    std::string_view name) {
  return std::make_unique<PluginHostModule>(std::string{name}, state);
}

std::unique_ptr<rtype::engine::IModule>
ApplicationPluginsFixture::bareHostNamed(std::string_view name) {
  return std::make_unique<ProvidingModule>(std::string{name}, bareService,
                                           bareBus);
}

std::unique_ptr<rtype::engine::Application>
ApplicationPluginsFixture::applicationOf(
    std::unique_ptr<rtype::engine::IModule> host,
    std::vector<std::string> lines) {
  lines.insert(lines.begin(),
               std::string{PLUGIN_FOLDER_LINE_START} + pluginFolderText());
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::move(host));
  return std::make_unique<rtype::engine::Application>(
      withoutStages(), std::move(modules),
      runningLines(std::move(lines), journal, refusals));
}

void ApplicationPluginsFixture::publishOneEvent() {
  state.bus.publish(TestEvent{});
  state.bus.dispatch();
}
