#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The settings of PlayerSettings a listener can be told about. */
enum class SettingId : std::uint8_t {
  MusicVolume,
  EffectsVolume,
  Muted,
  WindowSize,
  DisplayMode,
  Keys,
  ColorMode,
  ReduceFlashes,
  SoundCues,
  AutoFire,
};

}  // namespace rtype::client
