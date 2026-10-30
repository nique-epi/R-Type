#include "ClientAudioModule.hpp"
#include <memory>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "AudioMode.hpp"
#include "AudioService.hpp"
#include "ClientConstants.hpp"
#include "IAudioService.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "SettingsService.hpp"
#include "SilentAudioService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientAudioModule::ClientAudioModule(AudioMode mode)
    : mode_(mode), logger_(std::string{CLIENT_LOGGER_NAME}) {}

std::string ClientAudioModule::name() const {
  return std::string{AUDIO_MODULE_NAME};
}

std::vector<std::string> ClientAudioModule::dependencies() const {
  return {std::string{ASSETS_MODULE_NAME}, std::string{SETTINGS_MODULE_NAME}};
}

void ClientAudioModule::install(engine::InstallContext& context) {
  if (mode_ == AudioMode::Enabled) {
    service_ = std::make_unique<AudioService>(
        context.require<AssetLibrary>(), context.require<SettingsService>());
  } else {
    service_ = std::make_unique<SilentAudioService>();
  }
  context.provide<IAudioService>(*service_);
  context.addWork(UPDATE_STAGE, [this](engine::Duration elapsed) {
    service_->update(elapsed);
  });
}

void ClientAudioModule::start() {
  if (mode_ == AudioMode::Disabled) {
    logger_.info("sound is off: the client runs without sound");
  }
  service_->open();
}

void ClientAudioModule::stop() {}

}  // namespace rtype::client
