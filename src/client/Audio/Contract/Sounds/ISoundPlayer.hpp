#pragma once

#include <string_view>

namespace rtype::client {

/**
 * @brief Plays the short sounds of the client, each once from start to end: a
 * shot, an explosion, a sound of the intro.
 *
 * A sound is named by its asset id, one of SoundConstants.hpp, loaded at
 * launch with the interface assets. Whoever plays a sound never waits for it
 * nor stops it, so it can be tested with a double that writes the ids down.
 */
class ISoundPlayer {
 public:
  virtual ~ISoundPlayer() = default;

  /** @brief Starts the sound of an asset id at the VOICES volume of the
   * player for a sound of VOICE_SOUND_IDS, at its EFFECTS volume for any
   * other; nothing plays while MUTE ALL is on. */
  virtual void play(std::string_view soundId) = 0;

  /** @brief Reads the sound of an asset id that was not loaded at launch, a
   * sound a content pack brings, so it plays without reading the disk. A
   * sound it cannot read stays silent: play() then plays nothing for it. */
  virtual void load(std::string_view soundId) = 0;
};

}  // namespace rtype::client
