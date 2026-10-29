#pragma once

namespace rtype::client {

/** @brief The settings of PlayerSettings a listener can be told about. */
enum class SettingId {
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
