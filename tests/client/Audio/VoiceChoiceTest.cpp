#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "AudioConstants.hpp"
#include "AudioTestConstants.hpp"
#include "VoiceChoice.hpp"
#include "VoiceState.hpp"

using rtype::client::SOUND_VOICE_LIMIT;
using rtype::client::voiceForNextSound;
using rtype::client::VoiceState;
using rtype::client::testing::MANY_SOUNDS;
using rtype::client::testing::THREE_VOICES;

/**
 * Given no voice yet
 * When a sound comes
 * Then it opens the first voice
 */
TEST(VoiceChoice, TheFirstSoundOpensTheFirstVoice) {
  EXPECT_EQ(voiceForNextSound({}, THREE_VOICES), 0U);
}

/**
 * Given two voices, both playing, under a limit of three
 * When a sound comes
 * Then it opens a third voice and cuts neither
 */
TEST(VoiceChoice, ASoundOpensANewVoiceUnderTheLimit) {
  const std::array voices{VoiceState{.playing = true, .startRank = 0},
                          VoiceState{.playing = true, .startRank = 1}};

  EXPECT_EQ(voiceForNextSound(voices, THREE_VOICES), 2U);
}

/**
 * Given three voices at a limit of three, the second one silent
 * When a sound comes
 * Then it takes the silent voice
 */
TEST(VoiceChoice, ASoundTakesASilentVoiceAtTheLimit) {
  const std::array voices{VoiceState{.playing = true, .startRank = 0},
                          VoiceState{.playing = false, .startRank = 1},
                          VoiceState{.playing = true, .startRank = 2}};

  EXPECT_EQ(voiceForNextSound(voices, THREE_VOICES), 1U);
}

/**
 * Given three voices at a limit of three, all playing, the third started
 * first
 * When a sound comes
 * Then it cuts the third voice
 */
TEST(VoiceChoice, ASoundCutsTheSoundThatStartedFirstWhenEveryVoicePlays) {
  const std::array voices{VoiceState{.playing = true, .startRank = 4},
                          VoiceState{.playing = true, .startRank = 5},
                          VoiceState{.playing = true, .startRank = 3}};

  EXPECT_EQ(voiceForNextSound(voices, THREE_VOICES), 2U);
}

/**
 * Given a client none of whose sounds ever ends
 * When forty sounds come one after the other, each on the voice it is given
 * Then the client never holds more than SOUND_VOICE_LIMIT voices
 */
TEST(VoiceChoice, TheClientNeverPlaysMoreSoundsThanItsVoiceLimit) {
  std::vector<VoiceState> voices;
  for (std::uint64_t rank = 0; rank < MANY_SOUNDS; ++rank) {
    const std::size_t voice = voiceForNextSound(voices, SOUND_VOICE_LIMIT);
    if (voice == voices.size()) {
      voices.emplace_back();
    }
    voices.at(voice) = VoiceState{.playing = true, .startRank = rank};
  }

  EXPECT_EQ(voices.size(), SOUND_VOICE_LIMIT);
}
