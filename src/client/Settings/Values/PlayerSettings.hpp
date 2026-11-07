#pragma once

#include <optional>
#include "ColorMode.hpp"
#include "DisplayMode.hpp"
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
 * player picks a size: the window then opens at the largest one that fits. The
 * window opens windowed until the player picks another display mode.
 * clientPrediction is on: the ship moves before the server answers.
 * pilotNames is on: the nickname of each pilot is written over its ship.
 * chatShown is on: the chat of the session shows, and its key opens it.
 * chatLetters, chatClosedLines, chatOpenLines: the letters of a line of the
 * chat, and its lines while closed and while open. chatSeconds: how long a
 * line shows while the chat is closed. chatBackground: the opacity of the
 * layer under the open chat, in percent.
 */
struct PlayerSettings {
  int musicVolume = DEFAULT_MUSIC_VOLUME;
  int effectsVolume = DEFAULT_EFFECTS_VOLUME;
  bool muted = false;
  std::optional<PixelSize> windowSize;
  DisplayMode displayMode = DisplayMode::Windowed;
  KeyBindings keys;
  ColorMode colorMode = ColorMode::Off;
  bool reduceFlashes = false;
  bool soundCues = false;
  bool autoFire = false;
  bool clientPrediction = true;
  bool pilotNames = true;
  bool chatShown = true;
  int chatLetters = DEFAULT_CHAT_LETTERS;
  int chatClosedLines = DEFAULT_CHAT_CLOSED_LINES;
  int chatOpenLines = DEFAULT_CHAT_OPEN_LINES;
  int chatSeconds = DEFAULT_CHAT_SECONDS;
  int chatBackground = DEFAULT_CHAT_BACKGROUND;

  bool operator==(const PlayerSettings& other) const = default;
};

}  // namespace rtype::client
