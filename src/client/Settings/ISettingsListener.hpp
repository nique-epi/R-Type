#pragma once

#include "SettingId.hpp"

namespace rtype::client {

/** @brief A module that wants to know when a setting changes. */
class ISettingsListener {
 public:
  virtual ~ISettingsListener() = default;

  /** @brief Called once the setting @p changed holds its new value, which the
   * listener reads from the SettingsService; the settings read at launch tell
   * every setting, moved or not. */
  virtual void settingChanged(SettingId changed) = 0;
};

}  // namespace rtype::client
