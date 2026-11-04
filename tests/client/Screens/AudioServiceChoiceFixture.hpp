#pragma once

#include <gtest/gtest.h>
#include "IAudioService.hpp"
#include "LoopStages.hpp"
#include "ServiceRegistry.hpp"
#include "SilentAudioService.hpp"

/** @brief The registry the screens install over, with or without an audio
 * service provided by a module before them, and the silent service the
 * screens hold. */
class AudioServiceChoiceFixture : public ::testing::Test {
 protected:
  AudioServiceChoiceFixture();

  /** @brief Provides an audio service, as the sound module does when it is
   * listed before the screens. */
  void provideAnAudioService();

  /** @returns The service the screens choose when they install now. */
  [[nodiscard]] rtype::client::IAudioService& chosenByTheScreens();

  [[nodiscard]] rtype::client::IAudioService& provided();
  [[nodiscard]] rtype::client::IAudioService& silenceOfTheScreens();

 private:
  rtype::engine::ServiceRegistry services_;
  rtype::engine::LoopStages stages_;
  rtype::client::SilentAudioService provided_;
  rtype::client::SilentAudioService silence_;
};
