#pragma once

#include <string>
#include "IMasterAddress.hpp"
#include "ISettingsListener.hpp"
#include "MasterChoiceParts.hpp"
#include "SettingId.hpp"

namespace rtype::client {

/**
 * @brief Which master the server map reads: the one the player chose on the
 * map, kept in PlayerSettings::master, or else the one the master-url setting
 * names. Told of every change of the settings, it points the MasterLink at the
 * master of a new choice, without waiting for it, and asks the ServerCatalog
 * for the list again, so the map shows the servers of that master.
 */
class MasterChoice final : public IMasterAddress, public ISettingsListener {
 public:
  explicit MasterChoice(MasterChoiceParts parts);

  [[nodiscard]] std::string url() const override;

  /** @brief Points the link at the master of the player and asks for the list
   * again when @p changedSetting is SettingId::Master; nothing otherwise. */
  void settingChanged(SettingId changedSetting) override;

 private:
  MasterChoiceParts parts_;
};

}  // namespace rtype::client
