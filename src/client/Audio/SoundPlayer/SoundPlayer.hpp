#pragma once

#include <SFML/Audio/Sound.hpp>
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>
#include "AudioConstants.hpp"
#include "ISettingsListener.hpp"
#include "ISoundPlayer.hpp"
#include "SettingId.hpp"
#include "VoiceState.hpp"

namespace sf {
class SoundBuffer;
}  // namespace sf

namespace rtype::client {

class AssetLibrary;
class SettingsService;

/**
 * @brief Plays the sounds of the asset library on SOUND_VOICE_LIMIT voices at
 * most, each at the EFFECTS volume of the player, nothing while MUTE ALL is
 * on. It keeps that volume, and the settings service tells it when it moves.
 *
 * A voice is an sf::Sound, created the first time it is needed and kept; the
 * next sound takes the voice voiceForNextSound() gives, so a sound started
 * while every voice plays cuts the one that started first. The volumes are
 * of a sound is the one the settings had when the last of them changed, so
 * the options change the next sound. The voices are reserved at once, so a
 * voice never moves while it plays.
 */
class SoundPlayer : public ISoundPlayer, public ISettingsListener {
 public:
  /**
   * @param library Holds the sounds, loaded with the interface assets; it
   * must outlive the player.
   * @param settings Holds the volumes of the player; it must outlive the
   * player, and tell it through subscribe().
   */
  SoundPlayer(const AssetLibrary& library, const SettingsService& settings);

  /** @throws AssetNotLoadedException for a sound not loaded: every id of
   * ALL_SOUND_IDS is loaded at launch. */
  void play(std::string_view soundId) override;

  /** @brief Takes the volume of the sounds the settings now hold when
   * @p changed moves it; ignores the other settings. */
  void settingChanged(SettingId changed) override;

 private:
  /** @returns What voiceForNextSound() reads of each voice, in the first
   * voices_.size() places. */
  [[nodiscard]] std::array<VoiceState, SOUND_VOICE_LIMIT> voiceStates() const;

  /** @returns The voice the next sound plays on, stopped and given
   * @p buffer, created when it is new. */
  sf::Sound& voiceFor(const sf::SoundBuffer& buffer);

  const AssetLibrary* library_;
  const SettingsService* settings_;
  float volume_;
  std::vector<sf::Sound> voices_;
  std::vector<std::uint64_t> startRanks_;
  std::uint64_t startedSounds_{};
};

}  // namespace rtype::client
