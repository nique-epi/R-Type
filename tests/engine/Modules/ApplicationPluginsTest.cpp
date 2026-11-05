#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ApplicationPluginsFixtures.hpp"
#include "ApplicationPluginsTestConstants.hpp"
#include "EngineException.hpp"
#include "EventBus.hpp"
#include "IModule.hpp"
#include "PluginHostState.hpp"
#include "ProvidingModule.hpp"
#include "TestEvent.hpp"
#include "TestPluginConstants.hpp"
#include "TestService.hpp"

using rtype::engine::Application;
using rtype::engine::EventBus;
using rtype::engine::IModule;
using rtype::engine::MissingServiceException;

/**
 * Given a host module providing an EventBus, then the module of the probe
 * plugin, loaded from its shared library, both started
 * When the host publishes an event and dispatches it
 * Then the plugin receives it once
 */
TEST(ApplicationPlugins, APluginReceivesAnEventTheProgramPublished) {
  PluginHostState state;
  HandLoadedPlugin plugin{PROBE_PLUGIN_NAME};
  Application application(hostThenPlugin(state, plugin));
  application.start();

  state.bus.publish(TestEvent{});
  state.bus.dispatch();

  EXPECT_EQ(state.delivered.count, ONE_EVENT);
}

/**
 * Given a host that provides an EventBus but no DeliveredEvents, and the probe
 * plugin, which requires both
 * When the application starts
 * Then the install stops with the MissingServiceException the plugin threw
 */
TEST(ApplicationPlugins, AServiceAPluginMissesStopsTheInstallWithItsType) {
  TestService service;
  EventBus bus;
  HandLoadedPlugin plugin{PROBE_PLUGIN_NAME};
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(std::make_unique<ProvidingModule>(
      std::string{PLUGIN_HOST_MODULE_NAME}, service, bus));
  modules.push_back(plugin.takeModule());
  Application application(std::move(modules));

  EXPECT_THROW(application.start(), MissingServiceException);
}
