#include "AudioServiceChoiceFixture.hpp"
#include <memory>
#include <string>
#include "AudioMode.hpp"
#include "AudioServiceChoice.hpp"
#include "ClientAudioModule.hpp"
#include "ClientConstants.hpp"
#include "IAudioService.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"

using rtype::client::AUDIO_MODULE_NAME;
using rtype::client::AudioMode;
using rtype::client::ClientAudioModule;
using rtype::client::IAudioService;
using rtype::client::SCREENS_MODULE_NAME;
using rtype::engine::InstallContext;

AudioServiceChoiceFixture::AudioServiceChoiceFixture()
    : library_(folder_.path()),
      settingsService_(settings_, storage_),
      stages_(rtype::client::CLIENT_LOOP_STAGES) {
  services_.provide(library_);
  services_.provide(settingsService_);
}

void AudioServiceChoiceFixture::installTheSoundModule(AudioMode mode) {
  soundModule_ = std::make_unique<ClientAudioModule>(mode);
  InstallContext installing{std::string{AUDIO_MODULE_NAME}, services_, stages_};
  soundModule_->install(installing);
}

IAudioService& AudioServiceChoiceFixture::chosenByTheScreens() {
  const InstallContext installing{std::string{SCREENS_MODULE_NAME}, services_,
                                  stages_};
  return rtype::client::chooseAudioService(installing, silence_);
}

IAudioService* AudioServiceChoiceFixture::providedBySound() {
  return services_.find<IAudioService>();
}

IAudioService& AudioServiceChoiceFixture::silenceOfTheScreens() {
  return silence_;
}
