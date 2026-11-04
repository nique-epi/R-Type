#include "AudioServiceChoiceFixture.hpp"
#include <string>
#include "AudioServiceChoice.hpp"
#include "ClientConstants.hpp"
#include "IAudioService.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"

using rtype::client::IAudioService;

AudioServiceChoiceFixture::AudioServiceChoiceFixture()
    : stages_(rtype::client::CLIENT_LOOP_STAGES) {}

void AudioServiceChoiceFixture::provideAnAudioService() {
  services_.provide<IAudioService>(provided_);
}

IAudioService& AudioServiceChoiceFixture::chosenByTheScreens() {
  const rtype::engine::InstallContext installing{
      std::string{rtype::client::SCREENS_MODULE_NAME}, services_, stages_};
  return rtype::client::chooseAudioService(installing, silence_);
}

IAudioService& AudioServiceChoiceFixture::provided() { return provided_; }

IAudioService& AudioServiceChoiceFixture::silenceOfTheScreens() {
  return silence_;
}
