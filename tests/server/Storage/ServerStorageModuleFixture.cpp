#include "ServerStorageModuleFixture.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReader.hpp"
#include "IModule.hpp"
#include "ListeningPortModule.hpp"
#include "LoopStages.hpp"
#include "ServerConstants.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerStorageModule.hpp"
#include "SettingLine.hpp"

std::unique_ptr<rtype::engine::Application>
ServerStorageModuleFixture::storageApplication(
    std::optional<std::uint16_t> port, std::vector<std::string> lines) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(
      std::make_unique<rtype::server::ServerLoggerModule>(journalOptions()));
  if (port.has_value()) {
    modules.push_back(std::make_unique<ListeningPortModule>(*port));
  }
  modules.push_back(
      std::make_unique<rtype::server::ServerStorageModule>(folder()));
  return std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules),
      rtype::engine::configurationStep(
          folder() / rtype::server::SERVER_CONFIGURATION_FILE_NAME,
          std::move(lines), report_));
}

std::string ServerStorageModuleFixture::databaseLine(
    const std::filesystem::path& file) {
  return rtype::engine::settingLine(rtype::server::DATABASE_SETTING_NAME,
                                    file.string());
}

const std::filesystem::path& ServerStorageModuleFixture::folder() const {
  return folder_.path();
}
