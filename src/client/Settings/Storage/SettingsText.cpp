#include "SettingsText.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "InputConstants.hpp"
#include "KeyNames.hpp"
#include "NamedNumber.hpp"
#include "PixelSize.hpp"
#include "PlayerSettings.hpp"
#include "Setting.hpp"
#include "SettingRange.hpp"
#include "SettingsConstants.hpp"
#include "SettingsTextConstants.hpp"
#include "ShipLocker.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "ShipLookSlot.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

using identifiers::lookIndexOf;
using identifiers::lookNumberOf;
using identifiers::NO_SHIP_LOOK_ITEM;
using identifiers::SHIP_LOOK_ITEM_COUNT;
using identifiers::SHIP_LOOK_ITEM_NAMES;
using identifiers::SHIP_LOOK_ITEM_SLOTS;
using identifiers::ShipLookSlot;

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

/** @returns The whole number @p text holds when @p range takes it, or nothing:
 * an integer setting always has a range, so none takes nothing. */
std::optional<int> integerIn(std::string_view text,
                             const std::optional<SettingRange>& range) {
  const std::optional<int> value = integerOf(text);
  if (!value.has_value() || !range.has_value() || *value < range->minimum ||
      *value > range->maximum) {
    return std::nullopt;
  }
  return value;
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

template <typename Value, typename Field>
LineOutcome assign(std::optional<Value> parsed, Field& field) {
  if (!parsed.has_value()) {
    return LineOutcome::InvalidValue;
  }
  field = *parsed;
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

/** @returns What became of an integer setting, or nothing when the line names
 * another kind of setting. */
std::optional<LineOutcome> applyNumber(PlayerSettings& settings,
                                       SettingLine setting) {
  for (const std::span<const NamedNumber> numbers :
       {std::span<const NamedNumber>(VOLUME_NUMBERS),
        std::span<const NamedNumber>(CHAT_NUMBERS)}) {
    for (const NamedNumber& number : numbers) {
      if (setting.name == number.name) {
        return assign(integerIn(setting.value, number.setting->range),
                      settings.*number.setting->member);
      }
    }
  }
  return std::nullopt;
}

/** @returns What became of a setting that is an on or off switch, or nothing
 * when the line names another kind of setting. */
std::optional<LineOutcome> applySwitch(PlayerSettings& settings,
                                       SettingLine setting) {
  const auto [name, value] = setting;
  if (name == MUTED_NAME) {
    return assign(switchOf(value), settings.muted);
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
  if (name == CLIENT_PREDICTION_NAME) {
    return assign(switchOf(value), settings.clientPrediction);
  }
  if (name == PILOT_NAMES_NAME) {
    return assign(switchOf(value), settings.pilotNames);
  }
  if (name == CHAT_SHOWN_NAME) {
    return assign(switchOf(value), settings.chatShown);
  }
  return std::nullopt;
}

/** @returns The value of a slot of the ship in the file: the name of its
 * item, or NO_LOOK_ITEM_VALUE. */
std::string_view lookItemValue(std::uint8_t item) {
  if (item == NO_SHIP_LOOK_ITEM) {
    return NO_LOOK_ITEM_VALUE;
  }
  return SHIP_LOOK_ITEM_NAMES.at(lookIndexOf(item));
}

/** @returns What a slot of the ship holds for @p text: NO_SHIP_LOOK_ITEM for
 * NO_LOOK_ITEM_VALUE, the item of that name when it sits in @p slot, nothing
 * otherwise. */
std::optional<std::uint8_t> lookItemOf(std::string_view text,
                                       ShipLookSlot slot) {
  if (text == NO_LOOK_ITEM_VALUE) {
    return NO_SHIP_LOOK_ITEM;
  }
  for (std::size_t item = 0; item < SHIP_LOOK_ITEM_COUNT; ++item) {
    if (SHIP_LOOK_ITEM_NAMES.at(item) == text &&
        SHIP_LOOK_ITEM_SLOTS.at(item) == slot) {
      return lookNumberOf(item);
    }
  }
  return std::nullopt;
}

/** @returns What became of a slot of the ship, or nothing when the line names
 * another setting. */
std::optional<LineOutcome> applyLook(PlayerSettings& settings,
                                     SettingLine setting) {
  const auto [name, value] = setting;
  if (name == SHIP_HEAD_NAME) {
    return assign(lookItemOf(value, ShipLookSlot::Head),
                  settings.shipLook.head);
  }
  if (name == SHIP_EYES_NAME) {
    return assign(lookItemOf(value, ShipLookSlot::Eyes),
                  settings.shipLook.eyes);
  }
  if (name == SHIP_FACE_NAME) {
    return assign(lookItemOf(value, ShipLookSlot::Face),
                  settings.shipLook.face);
  }
  return std::nullopt;
}

/** @returns The locker the value @p text of the file describes: the items it
 * names, separated by LOCKER_ITEM_SEPARATOR; a name no item has is skipped,
 * and EMPTY_LOCKER_VALUE names none. */
ShipLocker lockerNamed(std::string_view text) {
  ShipLocker locker;
  locker.items.reset();
  std::size_t start = 0;
  while (start <= text.size()) {
    const std::size_t end =
        std::min(text.find(LOCKER_ITEM_SEPARATOR, start), text.size());
    const std::string_view name = text.substr(start, end - start);
    for (std::size_t item = 0; item < SHIP_LOOK_ITEM_COUNT; ++item) {
      if (SHIP_LOOK_ITEM_NAMES.at(item) == name) {
        locker.items.set(item);
      }
    }
    start = end + 1;
  }
  return locker;
}

/** @returns The value of the locker in the file: the names of the items it
 * holds, in the order of the catalog, or EMPTY_LOCKER_VALUE. */
std::string lockerValue(const ShipLocker& locker) {
  std::string value;
  for (std::size_t item = 0; item < SHIP_LOOK_ITEM_COUNT; ++item) {
    if (locker.items.test(item)) {
      if (!value.empty()) {
        value += LOCKER_ITEM_SEPARATOR;
      }
      value += SHIP_LOOK_ITEM_NAMES.at(item);
    }
  }
  return value.empty() ? std::string{EMPTY_LOCKER_VALUE} : value;
}

/** @returns Applied when the line is the locker, nothing when it names
 * another setting. */
std::optional<LineOutcome> applyLocker(PlayerSettings& settings,
                                       SettingLine setting) {
  if (setting.name != SHIP_LOCKER_NAME) {
    return std::nullopt;
  }
  settings.shipLocker = lockerNamed(setting.value);
  return LineOutcome::Applied;
}

/** @brief Applies @p setting when it names a setting written as a word or a
 * pair: the window size, the display and the color modes.
 * @returns What became of the line, nothing for any other name. */
std::optional<LineOutcome> applyValue(PlayerSettings& settings,
                                      SettingLine setting) {
  const auto [name, value] = setting;
  if (name == WINDOW_SIZE_NAME) {
    return assign(windowSizeOf(value), settings.windowSize);
  }
  if (name == DISPLAY_MODE_NAME) {
    return assign(valueNamed(value, DISPLAY_MODE_VALUES, DISPLAY_MODES),
                  settings.displayMode);
  }
  if (name == COLOR_MODE_NAME) {
    return assign(valueNamed(value, COLOR_MODE_VALUES, COLOR_MODES),
                  settings.colorMode);
  }
  return std::nullopt;
}

LineOutcome applySetting(PlayerSettings& settings, SettingLine setting) {
  const auto [name, value] = setting;
  if (name == SETTINGS_VERSION_NAME) {
    return integerOf(value) == SETTINGS_VERSION ? LineOutcome::Applied
                                                : LineOutcome::InvalidValue;
  }
  if (const std::optional<LineOutcome> outcome =
          applyNumber(settings, setting)) {
    return *outcome;
  }
  if (const std::optional<LineOutcome> outcome =
          applyValue(settings, setting)) {
    return *outcome;
  }
  if (const std::optional<LineOutcome> outcome =
          applySwitch(settings, setting)) {
    return *outcome;
  }
  if (const std::optional<LineOutcome> outcome = applyLook(settings, setting)) {
    return *outcome;
  }
  if (const std::optional<LineOutcome> outcome =
          applyLocker(settings, setting)) {
    return *outcome;
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

/** @brief Writes the lines of the settings of the chat. */
void writeChat(std::string& text, const PlayerSettings& settings) {
  writeLine(text, CHAT_SHOWN_NAME, switchValue(settings.chatShown));
  for (const NamedNumber& number : CHAT_NUMBERS) {
    writeLine(text, number.name,
              std::to_string(settings.*number.setting->member));
  }
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
  writeLine(
      text, DISPLAY_MODE_NAME,
      DISPLAY_MODE_VALUES[static_cast<std::size_t>(settings.displayMode)]);
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
  writeLine(text, CLIENT_PREDICTION_NAME,
            switchValue(settings.clientPrediction));
  writeLine(text, PILOT_NAMES_NAME, switchValue(settings.pilotNames));
  writeChat(text, settings);
  writeLine(text, SHIP_HEAD_NAME, lookItemValue(settings.shipLook.head));
  writeLine(text, SHIP_EYES_NAME, lookItemValue(settings.shipLook.eyes));
  writeLine(text, SHIP_FACE_NAME, lookItemValue(settings.shipLook.face));
  writeLine(text, SHIP_LOCKER_NAME, lockerValue(settings.shipLocker));
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
