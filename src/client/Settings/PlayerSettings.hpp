#pragma once

#include <optional>
#include "ColorMode.hpp"
#include "KeyBindings.hpp"
#include "PixelSize.hpp"
#include "SettingsConstants.hpp"

namespace rtype::client {

/**
 * @brief What the player sets in the options, kept from one launch to the
 * next.
 *
 * A default-built value holds the settings of a first launch, except
 * reduceFlashes, which the client then takes from the system. Volumes are in
 * percent, MINIMUM_VOLUME to MAXIMUM_VOLUME. windowSize stays empty until the
 * player picks a size: the window then opens at the largest one that fits.
 */
struct PlayerSettings {
  int musicVolume = DEFAULT_MUSIC_VOLUME;
  int effectsVolume = DEFAULT_EFFECTS_VOLUME;
  bool muted = false;
  std::optional<PixelSize> windowSize;
  KeyBindings keys;
  ColorMode colorMode = ColorMode::Off;
  bool reduceFlashes = false;
  bool soundCues = false;
  bool autoFire = false;

  bool operator==(const PlayerSettings& other) const = default;
};

}  // namespace rtype::client
