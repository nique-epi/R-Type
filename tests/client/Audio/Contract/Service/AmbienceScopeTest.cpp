#include <gtest/gtest.h>
#include "Ambience.hpp"
#include "AmbienceScope.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingSoundPlayer.hpp"

using rtype::client::Ambience;
using rtype::client::AmbienceScope;

/**
 * Given an audio service with no ambience
 * When a screen holds the space ambience
 * Then the space ambience plays
 */
TEST(AmbienceScope, HoldingAnAmbienceSetsIt) {
  RecordingSoundPlayer sounds;
  RecordingAudioService audio(sounds);

  const AmbienceScope scope(audio, Ambience::Space);

  EXPECT_EQ(audio.ambience(), Ambience::Space);
}

/**
 * Given a screen holding the space ambience
 * When it is destroyed
 * Then the ambience that played before comes back: none
 */
TEST(AmbienceScope, LeavingPutsBackTheAmbienceFoundOnEntry) {
  RecordingSoundPlayer sounds;
  RecordingAudioService audio(sounds);

  {
    const AmbienceScope scope(audio, Ambience::Space);
  }

  EXPECT_EQ(audio.ambience(), Ambience::None);
}

/**
 * Given the hangar opened over a map that holds the space ambience
 * When the hangar is closed
 * Then the map gets its space ambience back
 */
TEST(AmbienceScope, ALeftHangarGivesTheMapItsSpaceBack) {
  RecordingSoundPlayer sounds;
  RecordingAudioService audio(sounds);
  const AmbienceScope map(audio, Ambience::Space);

  {
    const AmbienceScope hangar(audio, Ambience::Hangar);
    EXPECT_EQ(audio.ambience(), Ambience::Hangar);
  }

  EXPECT_EQ(audio.ambience(), Ambience::Space);
}
