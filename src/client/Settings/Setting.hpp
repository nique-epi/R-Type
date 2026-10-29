#pragma once

#include <optional>
#include "PlayerSettings.hpp"
#include "SettingId.hpp"
#include "SettingRange.hpp"

namespace rtype::client {

/**
 * @brief One setting of PlayerSettings: the field it is kept in, the id its
 * listeners are told, and the values it accepts.
 *
 * @tparam Value The type of the field: int, bool or ColorMode.
 */
template <typename Value>
struct Setting {
  Value PlayerSettings::* member = nullptr;
  SettingId id = SettingId::MusicVolume;
  /** @brief Only an int setting reads it; empty accepts every value. */
  std::optional<SettingRange> range;
};

}  // namespace rtype::client
