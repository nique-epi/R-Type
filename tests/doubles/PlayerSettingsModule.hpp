#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "PlayerSettings.hpp"
#include "RecordingSettingsStorage.hpp"
#include "SettingsService.hpp"

/**
 * @brief Stands in for the settings module of the client: it goes by
 * SETTINGS_MODULE_NAME and provides the PlayerSettings it was built with and a
 * SettingsService over them, which saves to a RecordingSettingsStorage, so a
 * test never reads or writes the settings.ini of the machine.
 */
class PlayerSettingsModule : public rtype::engine::IModule {
 public:
  explicit PlayerSettingsModule(rtype::client::PlayerSettings settings);
  PlayerSettingsModule(const PlayerSettingsModule&) = delete;
  PlayerSettingsModule& operator=(const PlayerSettingsModule&) = delete;
  PlayerSettingsModule(PlayerSettingsModule&&) = delete;
  PlayerSettingsModule& operator=(PlayerSettingsModule&&) = delete;
  ~PlayerSettingsModule() override = default;

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  /** @returns The service a test changes the settings through. */
  [[nodiscard]] rtype::client::SettingsService& service();

 private:
  rtype::client::PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  rtype::client::SettingsService service_;
};
