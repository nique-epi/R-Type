#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "PlayerSettings.hpp"
#include "SettingsFile.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief The module of the settings of the player: the PlayerSettings every
 * module reads, and the SettingsService every module changes them through,
 * which saves them to the settings file.
 *
 * install() provides both, the settings still at their defaults. start()
 * writes the settings file in the journal, or that the system tells no
 * settings folder, then reads the file over the settings of a first launch,
 * whose flashes are reduced when the system asks for less motion. Nothing
 * stays open: the file is read and closed, and written again by a save only.
 */
class ClientSettingsModule final : public engine::IModule {
 public:
  /** @brief Finds the settings file in the settings folder the system tells,
   * if any; reads nothing. */
  ClientSettingsModule();

  /** @returns SETTINGS_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns LOGGER_MODULE_NAME: the file and what it holds are logged once
   * the journal is open. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  /** @brief Writes where the settings are kept, or that they last for this
   * launch only. */
  void logFileLocation() const;

  std::optional<std::filesystem::path> file_;
  SettingsFile storage_;
  PlayerSettings settings_;
  SettingsService service_;
  logging::Logger logger_;
};

}  // namespace rtype::client
