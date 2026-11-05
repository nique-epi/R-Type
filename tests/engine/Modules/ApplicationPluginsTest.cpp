#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Application.hpp"
#include "ApplicationConfigurationTestConstants.hpp"
#include "ApplicationPluginsFixture.hpp"
#include "ApplicationPluginsTestConstants.hpp"
#include "EngineException.hpp"
#include "LogLevel.hpp"
#include "PluginConstants.hpp"
#include "PluginFiles.hpp"
#include "RecordedLine.hpp"
#include "TestPluginConstants.hpp"

using rtype::engine::DuplicateModuleNameException;
using rtype::engine::InvalidPluginNameException;
using rtype::engine::MissingServiceException;
using rtype::engine::PluginApiVersionException;
using rtype::engine::PluginEntryNotFoundException;
using rtype::engine::PluginNotFoundException;

/**
 * Given a host module providing an EventBus, then the module of the probe
 * plugin, loaded from its shared library by a plugin line, both started
 * When the host publishes an event and dispatches it
 * Then the plugin receives it once
 */
TEST_F(ApplicationPluginsFixture, APluginReceivesAnEventTheProgramPublished) {
  const auto application =
      applicationOf(hostNamed(PLUGIN_HOST_MODULE_NAME),
                    {std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME});
  application->start();

  publishOneEvent();

  EXPECT_EQ(state.delivered.count, ONE_EVENT);
}

/**
 * Given a host that provides an EventBus but no DeliveredEvents, and the probe
 * plugin, which requires both
 * When the application starts
 * Then the install stops with the MissingServiceException the plugin threw
 */
TEST_F(ApplicationPluginsFixture,
       AServiceAPluginMissesStopsTheInstallWithItsType) {
  const auto application =
      applicationOf(bareHostNamed(PLUGIN_HOST_MODULE_NAME),
                    {std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME});

  EXPECT_THROW(application->start(), MissingServiceException);
}

/**
 * Given the probe plugin loaded and a line that turns it off
 * When the application starts and the host publishes an event
 * Then the plugin receives nothing: a module line sees a plugin
 */
TEST_F(ApplicationPluginsFixture, AModuleLineTurnsAPluginOff) {
  const auto application =
      applicationOf(hostNamed(PLUGIN_HOST_MODULE_NAME),
                    {std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME,
                     std::string{"module "} + PROBE_PLUGIN_NAME + " off"});
  application->start();

  publishOneEvent();

  EXPECT_EQ(state.delivered.count, NO_EVENT);
}

/**
 * Given the same plugin line twice
 * When the application starts and the host publishes an event
 * Then no line is refused and the plugin counts the event once
 */
TEST_F(ApplicationPluginsFixture, ThePluginLineRunsInEveryPassButLoadsOnce) {
  const std::string line = std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME;
  const auto application =
      applicationOf(hostNamed(PLUGIN_HOST_MODULE_NAME), {line, line});
  application->start();

  publishOneEvent();

  EXPECT_TRUE(refusals.empty());
  EXPECT_EQ(state.delivered.count, ONE_EVENT);
}

/**
 * Given a started application that loaded the probe plugin
 * When the application is destroyed
 * Then the module of the plugin was destroyed, and the library closed after it
 */
TEST_F(ApplicationPluginsFixture, TheLibraryClosesAfterItsModuleIsDestroyed) {
  auto application =
      applicationOf(hostNamed(PLUGIN_HOST_MODULE_NAME),
                    {std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME});
  application->start();

  application.reset();

  EXPECT_TRUE(state.lifetime.moduleDestroyed);
}

/**
 * Given a plugin line naming a plugin that has no file
 * When the application starts
 * Then the start stops with an error naming the plugin, before any install
 */
TEST_F(ApplicationPluginsFixture, AMissingPluginStopsTheStartAndNamesIt) {
  const auto application =
      applicationOf(hostNamed(PLUGIN_HOST_MODULE_NAME),
                    {std::string{PLUGIN_LINE_START} + GHOST_PLUGIN_NAME});

  try {
    application->start();
    ADD_FAILURE() << "the application started";
  } catch (const PluginNotFoundException& error) {
    EXPECT_NE(std::string{error.what()}.find(GHOST_PLUGIN_NAME),
              std::string::npos);
  }
  EXPECT_EQ(state.delivered.count, NO_EVENT);
  EXPECT_EQ(journal.calls, std::vector<std::string>{CONFIGURE_STEP});
}

/**
 * Given a plugin line naming a library that exports no rtypePluginEntry
 * When the application starts
 * Then the start stops with an error naming the plugin
 */
TEST_F(ApplicationPluginsFixture, ALibraryWithoutEntryStopsTheStartAndNamesIt) {
  const auto application =
      applicationOf(hostNamed(PLUGIN_HOST_MODULE_NAME),
                    {std::string{PLUGIN_LINE_START} + ENTRYLESS_PLUGIN_NAME});

  try {
    application->start();
    ADD_FAILURE() << "the application started";
  } catch (const PluginEntryNotFoundException& error) {
    EXPECT_NE(std::string{error.what()}.find(ENTRYLESS_PLUGIN_NAME),
              std::string::npos);
  }
}

/**
 * Given a plugin line naming a plugin built for a later plugin API version
 * When the application starts
 * Then the start stops with an error naming the plugin and both versions
 */
TEST_F(ApplicationPluginsFixture,
       APluginOfAnotherApiVersionStopsTheStartWithBothVersions) {
  const auto application =
      applicationOf(hostNamed(PLUGIN_HOST_MODULE_NAME),
                    {std::string{PLUGIN_LINE_START} + FUTURE_PLUGIN_NAME});

  try {
    application->start();
    ADD_FAILURE() << "the application started";
  } catch (const PluginApiVersionException& error) {
    const std::string message{error.what()};
    EXPECT_NE(message.find(FUTURE_PLUGIN_NAME), std::string::npos);
    EXPECT_NE(message.find(std::to_string(FUTURE_PLUGIN_API_VERSION)),
              std::string::npos);
    EXPECT_NE(message.find(std::to_string(rtype::engine::PLUGIN_API_VERSION)),
              std::string::npos);
  }
}

/**
 * Given a plugin line whose name has a slash in it
 * When the application starts
 * Then the start stops: a plugin name never leaves the plugin folder
 */
TEST_F(ApplicationPluginsFixture, APluginNameWithASlashStopsTheStart) {
  const auto application = applicationOf(
      hostNamed(PLUGIN_HOST_MODULE_NAME),
      {std::string{PLUGIN_LINE_START} + PLUGIN_NAME_WITH_A_SLASH});

  EXPECT_THROW(application->start(), InvalidPluginNameException);
}

/**
 * Given a host module named like the plugin a line loads
 * When the application starts
 * Then the start stops: two modules never share a name
 */
TEST_F(ApplicationPluginsFixture, APluginWhoseNameAModuleHasStopsTheStart) {
  const auto application =
      applicationOf(hostNamed(PROBE_PLUGIN_NAME),
                    {std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME});

  EXPECT_THROW(application->start(), DuplicateModuleNameException);
}

/**
 * Given a plugin-folder line after a plugin line
 * When the application starts and the host publishes an event
 * Then that line is refused and the plugin, loaded before it, still works
 */
TEST_F(ApplicationPluginsFixture, APluginFolderLineAfterAPluginLineIsRefused) {
  const auto application = applicationOf(
      hostNamed(PLUGIN_HOST_MODULE_NAME),
      {std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME,
       std::string{PLUGIN_FOLDER_LINE_START} + OTHER_PLUGIN_FOLDER});
  application->start();

  publishOneEvent();

  ASSERT_EQ(refusals.size(), 1U);
  EXPECT_NE(refusals.front().find(FOLDER_AFTER_PLUGIN_REFUSAL),
            std::string::npos);
  EXPECT_EQ(state.delivered.count, ONE_EVENT);
}

/**
 * Given a plugin line, then a plugin-folder line giving the folder again
 * When the application starts
 * Then no line is refused and the plugin works
 */
TEST_F(ApplicationPluginsFixture,
       APluginFolderLineGivingTheSameFolderAgainDoesNothing) {
  const auto application = applicationOf(
      hostNamed(PLUGIN_HOST_MODULE_NAME),
      {std::string{PLUGIN_LINE_START} + PROBE_PLUGIN_NAME,
       std::string{PLUGIN_FOLDER_LINE_START} + pluginFolderText()});
  application->start();

  publishOneEvent();

  EXPECT_TRUE(refusals.empty());
  EXPECT_EQ(state.delivered.count, ONE_EVENT);
}

/**
 * Given a host named like the logger module, which provides a journal, and the
 * greeter plugin, which writes a line when it starts
 * When the application starts
 * Then the journal of the host holds that line, written by the plugin
 */
TEST_F(ApplicationPluginsFixture, TheProgramJournalHoldsTheLineAPluginWrote) {
  const auto application =
      applicationOf(hostNamed(LOGGER_MODULE_NAME_IN_TESTS),
                    {std::string{PLUGIN_LINE_START} + GREETER_PLUGIN_NAME});

  application->start();

  EXPECT_EQ(state.journal.lines(), (std::vector<RecordedLine>{RecordedLine{
                                       .level = rtype::logging::LogLevel::Info,
                                       .loggerName = GREETER_LOGGER,
                                       .body = GREETER_START_LINE}}));
}
