#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The settings of PlayerSettings a listener can be told about. */
enum class SettingId : std::uint8_t {
  MusicVolume,
  EffectsVolume,
  VoicesVolume,
  Muted,
  WindowSize,
  DisplayMode,
  Keys,
  ColorMode,
  ReduceFlashes,
  SoundCues,
  AutoFire,
  PilotNames,
  ChatShown,
  ChatLetters,
  ChatClosedLines,
  ChatOpenLines,
  ChatSeconds,
  ChatBackground,
  LagometerScale,
  LagometerBackground,
  ShipLook,
  ShipLocker,
  Kopecks,
  Chips,
};

}  // namespace rtype::client
