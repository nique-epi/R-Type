#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "AudioMode.hpp"
#include "AudioTestConstants.hpp"
#include "ClientAudioModule.hpp"
#include "ClientAudioModuleFixture.hpp"
#include "EngineException.hpp"
#include "IAudioService.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::ASSETS_MODULE_NAME;
using rtype::client::AudioMode;
using rtype::client::ClientAudioModule;
using rtype::client::IAudioService;
using rtype::client::SETTINGS_MODULE_NAME;
using rtype::client::testing::UNKNOWN_SOUND_ID;
using rtype::engine::InstallContext;
using rtype::engine::MissingServiceException;

/**
 * Given the sound module of a client without sound
 * When it is installed
 * Then the modules after it find an audio service
 */
TEST_F(ClientAudioModuleFixture, DisabledModuleProvidesAnAudioService) {
  ClientAudioModule module(AudioMode::Disabled);
  InstallContext installing = context();

  module.install(installing);

  EXPECT_NE(services().find<IAudioService>(), nullptr);
}

/**
 * Given the sound module of a client without sound, installed and started
 * When a screen plays a sound no asset has
 * Then nothing fails, and the screen checked nothing
 */
TEST_F(ClientAudioModuleFixture, DisabledModuleLetsAScreenPlayAnySound) {
  ClientAudioModule module(AudioMode::Disabled);
  InstallContext installing = context();
  module.install(installing);
  module.start();

  EXPECT_NO_THROW(
      services().find<IAudioService>()->sounds().play(UNKNOWN_SOUND_ID));
}

/**
 * Given the sound module of a client without sound, installed
 * When the loop runs a frame
 * Then the work the module added to the loop runs without failing
 */
TEST_F(ClientAudioModuleFixture, DisabledModuleUpdatesWithTheLoop) {
  ClientAudioModule module(AudioMode::Disabled);
  InstallContext installing = context();
  module.install(installing);
  module.start();

  EXPECT_NO_THROW(stages().run(ONE_FRAME));
}

/**
 * Given a registry that holds neither the assets nor the settings
 * When the sound module of a client with sound is installed
 * Then a MissingServiceException is thrown
 */
TEST_F(ClientAudioModuleFixture, EnabledModuleRequiresTheAssetsAndTheSettings) {
  ClientAudioModule module(AudioMode::Enabled);
  InstallContext installing = contextWithoutServices();

  EXPECT_THROW(module.install(installing), MissingServiceException);
}

/**
 * Given the sound module
 * When its dependencies are read
 * Then it comes after the assets and the settings it is built from
 */
TEST_F(ClientAudioModuleFixture, ModuleDependsOnTheAssetsAndTheSettings) {
  const ClientAudioModule module(AudioMode::Disabled);

  const std::vector<std::string> dependencies = module.dependencies();

  EXPECT_EQ(dependencies,
            (std::vector<std::string>{std::string{ASSETS_MODULE_NAME},
                                      std::string{SETTINGS_MODULE_NAME}}));
}
