#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include "ColorMode.hpp"
#include "DisplayMode.hpp"
#include "PixelSize.hpp"
#include "PlayerSettings.hpp"
#include "Setting.hpp"
#include "SettingId.hpp"
#include "SettingRange.hpp"
#include "SettingsConstants.hpp"
#include "ShipLocker.hpp"
#include "ShipLook.hpp"
#include "TextSize.hpp"

namespace rtype::client {

constexpr SettingRange VOLUME_SETTING_RANGE{.minimum = MINIMUM_VOLUME,
                                            .maximum = MAXIMUM_VOLUME};

constexpr Setting<int> MUSIC_VOLUME_SETTING{
    .member = &PlayerSettings::musicVolume,
    .id = SettingId::MusicVolume,
    .range = VOLUME_SETTING_RANGE};
constexpr Setting<int> EFFECTS_VOLUME_SETTING{
    .member = &PlayerSettings::effectsVolume,
    .id = SettingId::EffectsVolume,
    .range = VOLUME_SETTING_RANGE};
constexpr Setting<bool> MUTED_SETTING{.member = &PlayerSettings::muted,
                                      .id = SettingId::Muted,
                                      .range = std::nullopt};
constexpr Setting<std::optional<PixelSize>> WINDOW_SIZE_SETTING{
    .member = &PlayerSettings::windowSize,
    .id = SettingId::WindowSize,
    .range = std::nullopt};
constexpr Setting<DisplayMode> DISPLAY_MODE_SETTING{
    .member = &PlayerSettings::displayMode,
    .id = SettingId::DisplayMode,
    .range = std::nullopt};
constexpr Setting<ColorMode> COLOR_MODE_SETTING{
    .member = &PlayerSettings::colorMode,
    .id = SettingId::ColorMode,
    .range = std::nullopt};
constexpr Setting<bool> REDUCE_FLASHES_SETTING{
    .member = &PlayerSettings::reduceFlashes,
    .id = SettingId::ReduceFlashes,
    .range = std::nullopt};
constexpr Setting<bool> SOUND_CUES_SETTING{.member = &PlayerSettings::soundCues,
                                           .id = SettingId::SoundCues,
                                           .range = std::nullopt};
constexpr Setting<bool> AUTO_FIRE_SETTING{.member = &PlayerSettings::autoFire,
                                          .id = SettingId::AutoFire,
                                          .range = std::nullopt};
constexpr Setting<bool> PILOT_NAMES_SETTING{
    .member = &PlayerSettings::pilotNames,
    .id = SettingId::PilotNames,
    .range = std::nullopt};
constexpr Setting<TextSize> TEXT_SIZE_SETTING{
    .member = &PlayerSettings::textSize,
    .id = SettingId::TextSize,
    .range = std::nullopt};
constexpr Setting<bool> HIGH_CONTRAST_SETTING{
    .member = &PlayerSettings::highContrast,
    .id = SettingId::HighContrast,
    .range = std::nullopt};
constexpr Setting<identifiers::ShipLook> SHIP_LOOK_SETTING{
    .member = &PlayerSettings::shipLook,
    .id = SettingId::ShipLook,
    .range = std::nullopt};

/** @brief The ranges of the integer settings of the chat. */
constexpr SettingRange CHAT_LETTERS_RANGE{.minimum = MINIMUM_CHAT_LETTERS,
                                          .maximum = MAXIMUM_CHAT_LETTERS};
constexpr SettingRange CHAT_CLOSED_LINES_RANGE{
    .minimum = MINIMUM_CHAT_CLOSED_LINES, .maximum = MAXIMUM_CHAT_CLOSED_LINES};
constexpr SettingRange CHAT_OPEN_LINES_RANGE{
    .minimum = MINIMUM_CHAT_OPEN_LINES, .maximum = MAXIMUM_CHAT_OPEN_LINES};
constexpr SettingRange CHAT_SECONDS_RANGE{.minimum = MINIMUM_CHAT_SECONDS,
                                          .maximum = MAXIMUM_CHAT_SECONDS};
constexpr SettingRange CHAT_BACKGROUND_RANGE{
    .minimum = MINIMUM_CHAT_BACKGROUND, .maximum = MAXIMUM_CHAT_BACKGROUND};

constexpr Setting<bool> CHAT_SHOWN_SETTING{.member = &PlayerSettings::chatShown,
                                           .id = SettingId::ChatShown,
                                           .range = std::nullopt};
constexpr Setting<int> CHAT_LETTERS_SETTING{
    .member = &PlayerSettings::chatLetters,
    .id = SettingId::ChatLetters,
    .range = CHAT_LETTERS_RANGE};
constexpr Setting<int> CHAT_CLOSED_LINES_SETTING{
    .member = &PlayerSettings::chatClosedLines,
    .id = SettingId::ChatClosedLines,
    .range = CHAT_CLOSED_LINES_RANGE};
constexpr Setting<int> CHAT_OPEN_LINES_SETTING{
    .member = &PlayerSettings::chatOpenLines,
    .id = SettingId::ChatOpenLines,
    .range = CHAT_OPEN_LINES_RANGE};
constexpr Setting<int> CHAT_SECONDS_SETTING{
    .member = &PlayerSettings::chatSeconds,
    .id = SettingId::ChatSeconds,
    .range = CHAT_SECONDS_RANGE};
constexpr Setting<int> CHAT_BACKGROUND_SETTING{
    .member = &PlayerSettings::chatBackground,
    .id = SettingId::ChatBackground,
    .range = CHAT_BACKGROUND_RANGE};

/** @brief The ranges and the settings of the lagometer. */
constexpr SettingRange LAGOMETER_SCALE_RANGE{
    .minimum = MINIMUM_LAGOMETER_SCALE, .maximum = MAXIMUM_LAGOMETER_SCALE};
constexpr SettingRange LAGOMETER_BACKGROUND_RANGE{
    .minimum = MINIMUM_LAGOMETER_BACKGROUND,
    .maximum = MAXIMUM_LAGOMETER_BACKGROUND};
constexpr Setting<int> LAGOMETER_SCALE_SETTING{
    .member = &PlayerSettings::lagometerScale,
    .id = SettingId::LagometerScale,
    .range = LAGOMETER_SCALE_RANGE};
constexpr Setting<int> LAGOMETER_BACKGROUND_SETTING{
    .member = &PlayerSettings::lagometerBackground,
    .id = SettingId::LagometerBackground,
    .range = LAGOMETER_BACKGROUND_RANGE};

constexpr Setting<ShipLocker> SHIP_LOCKER_SETTING{
    .member = &PlayerSettings::shipLocker,
    .id = SettingId::ShipLocker,
    .range = std::nullopt};

/** @brief The balance of the player: none to MAXIMUM_KOPECKS. */
constexpr SettingRange KOPECKS_RANGE{
    .minimum = 0, .maximum = static_cast<int>(MAXIMUM_KOPECKS)};
constexpr Setting<std::uint32_t> KOPECKS_SETTING{
    .member = &PlayerSettings::kopecks,
    .id = SettingId::Kopecks,
    .range = KOPECKS_RANGE};

/** @brief Every setting a listener can be told about. */
constexpr std::array ALL_SETTING_IDS{
    SettingId::MusicVolume,
    SettingId::EffectsVolume,
    SettingId::Muted,
    SettingId::WindowSize,
    SettingId::DisplayMode,
    SettingId::Keys,
    SettingId::ColorMode,
    SettingId::ReduceFlashes,
    SettingId::SoundCues,
    SettingId::AutoFire,
    SettingId::PilotNames,
    SettingId::TextSize,
    SettingId::HighContrast,
    SettingId::ChatShown,
    SettingId::ChatLetters,
    SettingId::ChatClosedLines,
    SettingId::ChatOpenLines,
    SettingId::ChatSeconds,
    SettingId::ChatBackground,
    SettingId::LagometerScale,
    SettingId::LagometerBackground,
    SettingId::ShipLook,
    SettingId::ShipLocker,
    SettingId::Kopecks,
};

}  // namespace rtype::client
