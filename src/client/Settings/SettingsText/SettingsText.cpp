#include "SettingsText.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "InputConstants.hpp"
#include "KeyNames.hpp"
#include "PixelSize.hpp"
#include "PlayerSettings.hpp"
#include "SettingsConstants.hpp"

namespace rtype::client {

namespace {

/** @brief What became of one line of a settings text. */
enum class LineOutcome : std::uint8_t {
  Applied,
  WithoutSeparator,
  UnknownSetting,
  InvalidValue,
};

void writeLine(std::string& text, std::string_view name,
               std::string_view value) {
  text += name;
  text += SETTING_SEPARATOR;
  text += value;
  text += LINE_END;
}

std::string_view switchValue(bool switchedOn) {
  return switchedOn ? SWITCH_ON_VALUE : SWITCH_OFF_VALUE;
}

std::string windowSizeValue(PixelSize size) {
  return std::to_string(size.width) + WINDOW_SIZE_VALUE_SEPARATOR +
         std::to_string(size.height);
}

std::string_view trimmed(std::string_view text) {
  const std::size_t first = text.find_first_not_of(LINE_SPACES);
  if (first == std::string_view::npos) {
    return {};
  }
  const std::size_t last = text.find_last_not_of(LINE_SPACES);
  return text.substr(first, last - first + 1);
}

std::optional<int> integerOf(std::string_view text) {
  const char* const first = std::to_address(text.begin());
  const char* const last = std::to_address(text.end());
  int value = 0;
  const auto [stop, error] = std::from_chars(first, last, value);
  if (error != std::errc() || stop != last) {
    return std::nullopt;
  }
  return value;
}

std::optional<int> volumeOf(std::string_view text) {
  const std::optional<int> volume = integerOf(text);
  if (!volume.has_value() || *volume < MINIMUM_VOLUME ||
      *volume > MAXIMUM_VOLUME) {
    return std::nullopt;
  }
  return volume;
}

std::optional<bool> switchOf(std::string_view text) {
  if (text == SWITCH_ON_VALUE) {
    return true;
  }
  if (text == SWITCH_OFF_VALUE) {
    return false;
  }
  return std::nullopt;
}

std::optional<PixelSize> windowSizeOf(std::string_view text) {
  const std::size_t separator = text.find(WINDOW_SIZE_VALUE_SEPARATOR);
  if (separator == std::string_view::npos) {
    return std::nullopt;
  }
  const std::optional<int> width = integerOf(text.substr(0, separator));
  const std::optional<int> height = integerOf(text.substr(separator + 1));
  if (!width.has_value() || !height.has_value() || *width <= 0 ||
      *height <= 0) {
    return std::nullopt;
  }
  return PixelSize{.width = static_cast<unsigned int>(*width),
                   .height = static_cast<unsigned int>(*height)};
}

/** @returns The value placed in its list as this text is placed in the list of
 * names, or nothing when no name is this text. */
template <typename Value, std::size_t Count>
std::optional<Value> valueNamed(
    std::string_view text, const std::array<std::string_view, Count>& names,
    const std::array<Value, Count>& values) {
  for (std::size_t index = 0; index < Count; ++index) {
    if (names[index] == text) {
      return values[index];
    }
  }
  return std::nullopt;
}

template <typename Value, typename Setting>
LineOutcome assign(std::optional<Value> parsed, Setting& setting) {
  if (!parsed.has_value()) {
    return LineOutcome::InvalidValue;
  }
  setting = *parsed;
  return LineOutcome::Applied;
}

LineOutcome applyKey(PlayerSettings& settings, BoundAction action,
                     std::string_view value) {
  const std::optional<sf::Keyboard::Scancode> key = keyNamed(value);
  if (!key.has_value()) {
    return LineOutcome::InvalidValue;
  }
  settings.keys.bind(action, *key);
  return LineOutcome::Applied;
}

/** @brief A line of a settings text: its name and its value, without the
 * spaces around them. */
struct SettingLine {
  std::string_view name;
  std::string_view value;
};

/** @returns The name and the value of a line, or nothing when it holds no
 * SETTING_SEPARATOR. */
std::optional<SettingLine> settingLineOf(std::string_view line) {
  const std::size_t separator = line.find(SETTING_SEPARATOR);
  if (separator == std::string_view::npos) {
    return std::nullopt;
  }
  return SettingLine{.name = trimmed(line.substr(0, separator)),
                     .value = trimmed(line.substr(separator + 1))};
}

/** @returns What became of a setting written into the settings: applied, or
 * left out with the reason. */
LineOutcome applySetting(PlayerSettings& settings, SettingLine setting) {
  const auto [name, value] = setting;
  if (name == SETTINGS_VERSION_NAME) {
    return integerOf(value) == SETTINGS_VERSION ? LineOutcome::Applied
                                                : LineOutcome::InvalidValue;
  }
  if (name == MUSIC_VOLUME_NAME) {
    return assign(volumeOf(value), settings.musicVolume);
  }
  if (name == EFFECTS_VOLUME_NAME) {
    return assign(volumeOf(value), settings.effectsVolume);
  }
  if (name == MUTED_NAME) {
    return assign(switchOf(value), settings.muted);
  }
  if (name == WINDOW_SIZE_NAME) {
    return assign(windowSizeOf(value), settings.windowSize);
  }
  if (name == COLOR_MODE_NAME) {
    return assign(valueNamed(value, COLOR_MODE_VALUES, COLOR_MODES),
                  settings.colorMode);
  }
  if (name == REDUCE_FLASHES_NAME) {
    return assign(switchOf(value), settings.reduceFlashes);
  }
  if (name == SOUND_CUES_NAME) {
    return assign(switchOf(value), settings.soundCues);
  }
  if (name == AUTO_FIRE_NAME) {
    return assign(switchOf(value), settings.autoFire);
  }
  const std::optional<BoundAction> action =
      valueNamed(name, BOUND_ACTION_KEY_NAMES, BOUND_ACTIONS);
  if (!action.has_value()) {
    return LineOutcome::UnknownSetting;
  }
  return applyKey(settings, *action, value);
}

LineOutcome applyLine(PlayerSettings& settings, std::string_view line) {
  const std::optional<SettingLine> setting = settingLineOf(line);
  if (!setting.has_value()) {
    return LineOutcome::WithoutSeparator;
  }
  return applySetting(settings, *setting);
}

std::string ignoredLine(std::size_t number, LineOutcome outcome) {
  std::string text(LINE_PREFIX);
  text += std::to_string(number);
  text += LINE_REASON_SEPARATOR;
  switch (outcome) {
    case LineOutcome::WithoutSeparator:
      text += LINE_WITHOUT_SEPARATOR;
      break;
    case LineOutcome::UnknownSetting:
      text += UNKNOWN_SETTING;
      break;
    case LineOutcome::InvalidValue:
    case LineOutcome::Applied:
      text += INVALID_SETTING_VALUE;
      break;
  }
  return text;
}

}  // namespace

std::string settingsText(const PlayerSettings& settings) {
  std::string text(SETTINGS_FILE_COMMENT);
  text += LINE_END;
  writeLine(text, SETTINGS_VERSION_NAME, std::to_string(SETTINGS_VERSION));
  writeLine(text, MUSIC_VOLUME_NAME, std::to_string(settings.musicVolume));
  writeLine(text, EFFECTS_VOLUME_NAME, std::to_string(settings.effectsVolume));
  writeLine(text, MUTED_NAME, switchValue(settings.muted));
  if (settings.windowSize.has_value()) {
    writeLine(text, WINDOW_SIZE_NAME, windowSizeValue(*settings.windowSize));
  }
  for (std::size_t action = 0; action < BOUND_ACTION_COUNT; ++action) {
    writeLine(text, BOUND_ACTION_KEY_NAMES[action],
              keyName(settings.keys.keyOf(BOUND_ACTIONS[action]))
                  .value_or(UNNAMED_KEY_TEXT));
  }
  writeLine(text, COLOR_MODE_NAME,
            COLOR_MODE_VALUES[static_cast<std::size_t>(settings.colorMode)]);
  writeLine(text, REDUCE_FLASHES_NAME, switchValue(settings.reduceFlashes));
  writeLine(text, SOUND_CUES_NAME, switchValue(settings.soundCues));
  writeLine(text, AUTO_FIRE_NAME, switchValue(settings.autoFire));
  return text;
}

SettingsReading readSettingsText(std::string_view text,
                                 const PlayerSettings& defaults) {
  SettingsReading reading{.settings = defaults, .ignoredLines = {}};
  std::size_t lineNumber = 0;
  std::size_t lineStart = 0;
  while (lineStart <= text.size()) {
    const std::size_t lineEnd =
        std::min(text.find(LINE_END, lineStart), text.size());
    ++lineNumber;
    const std::string_view line =
        trimmed(text.substr(lineStart, lineEnd - lineStart));
    lineStart = lineEnd + 1;
    if (line.empty() || line.front() == COMMENT_START) {
      continue;
    }
    const LineOutcome outcome = applyLine(reading.settings, line);
    if (outcome != LineOutcome::Applied) {
      reading.ignoredLines.push_back(ignoredLine(lineNumber, outcome));
    }
  }
  return reading;
}

}  // namespace rtype::client
