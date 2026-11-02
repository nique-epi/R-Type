#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ClientConstants.hpp"
#include "ClientSettingsModule.hpp"
#include "InstallContext.hpp"
#include "LoopStages.hpp"
#include "ModuleConstants.hpp"
#include "RecordingSettingsListener.hpp"
#include "ServiceRegistry.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingId.hpp"
#include "SettingsService.hpp"

using rtype::client::ALL_SETTING_IDS;
using rtype::client::CLIENT_LOOP_STAGES;
using rtype::client::ClientSettingsModule;
using rtype::client::SettingId;
using rtype::client::SETTINGS_MODULE_NAME;
using rtype::client::SettingsService;
using rtype::engine::InstallContext;
using rtype::engine::LoopStages;
using rtype::engine::ServiceRegistry;

/**
 * Given the settings module installed and a listener subscribed to its service,
 * as the sound module does while it installs
 * When the module starts and reads the settings file, whatever it holds
 * Then the listener is told every setting
 */
TEST(ClientSettingsModuleTest, StartingTellsTheListenersEverySetting) {
  ServiceRegistry services;
  LoopStages stages(CLIENT_LOOP_STAGES);
  ClientSettingsModule module;
  InstallContext installing(std::string{SETTINGS_MODULE_NAME}, services,
                            stages);
  module.install(installing);
  RecordingSettingsListener listener;
  installing.require<SettingsService>().subscribe(listener);

  module.start();

  EXPECT_EQ(listener.changes(), std::vector<SettingId>(ALL_SETTING_IDS.begin(),
                                                       ALL_SETTING_IDS.end()));
}
