#pragma once

#include <gtest/gtest.h>
#include <memory>
#include "AssetLibrary.hpp"
#include "AudioMode.hpp"
#include "ClientAudioModule.hpp"
#include "IAudioService.hpp"
#include "LoopStages.hpp"
#include "PlayerSettings.hpp"
#include "RecordingSettingsStorage.hpp"
#include "ServiceRegistry.hpp"
#include "SettingsService.hpp"
#include "SilentAudioService.hpp"
#include "TemporaryFolder.hpp"

/** @brief The services the screens install over: an asset library on an empty
 * folder and the settings of a first launch, with or without the sound module
 * installed before them, and the silent service the screens hold. */
class AudioServiceChoiceFixture : public ::testing::Test {
 protected:
  AudioServiceChoiceFixture();

  /** @brief Installs the sound module in @p mode over the services, as
   * clientModules() lists it before the screens. */
  void installTheSoundModule(rtype::client::AudioMode mode);

  /** @returns The service the screens choose when they install now. */
  [[nodiscard]] rtype::client::IAudioService& chosenByTheScreens();

  /** @returns The service the sound module provided, or nullptr. */
  [[nodiscard]] rtype::client::IAudioService* providedBySound();

  [[nodiscard]] rtype::client::IAudioService& silenceOfTheScreens();

 private:
  TemporaryFolder folder_;
  rtype::client::AssetLibrary library_;
  rtype::client::PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  rtype::client::SettingsService settingsService_;
  rtype::engine::ServiceRegistry services_;
  rtype::engine::LoopStages stages_;
  std::unique_ptr<rtype::client::ClientAudioModule> soundModule_;
  rtype::client::SilentAudioService silence_;
};
