#pragma once

#include <array>
#include <optional>
#include "ColorMode.hpp"
#include "DisplayMode.hpp"
#include "PixelSize.hpp"
#include "PlayerSettings.hpp"
#include "Setting.hpp"
#include "SettingId.hpp"
#include "SettingRange.hpp"
#include "SettingsConstants.hpp"

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
constexpr Setting<bool> CLIENT_PREDICTION_SETTING{
    .member = &PlayerSettings::clientPrediction,
    .id = SettingId::ClientPrediction,
    .range = std::nullopt};

/** @brief Every setting a listener can be told about. */
constexpr std::array ALL_SETTING_IDS{
    SettingId::MusicVolume, SettingId::EffectsVolume,    SettingId::Muted,
    SettingId::WindowSize,  SettingId::DisplayMode,      SettingId::Keys,
    SettingId::ColorMode,   SettingId::ReduceFlashes,    SettingId::SoundCues,
    SettingId::AutoFire,    SettingId::ClientPrediction,
};

}  // namespace rtype::client
