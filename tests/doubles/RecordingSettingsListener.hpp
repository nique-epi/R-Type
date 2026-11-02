#pragma once

#include <vector>
#include "ISettingsListener.hpp"
#include "SettingId.hpp"

/** @brief Settings listener that writes down each change it is told about. */
class RecordingSettingsListener : public rtype::client::ISettingsListener {
 public:
  void settingChanged(rtype::client::SettingId changed) override;

  /** @brief Forgets the changes told so far. */
  void forgetChanges();

  /** @returns Every change told, in order. */
  [[nodiscard]] const std::vector<rtype::client::SettingId>& changes() const;

 private:
  std::vector<rtype::client::SettingId> changes_;
};
