#include "PlayerSettingsModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PlayerSettings.hpp"
#include "SettingsService.hpp"

PlayerSettingsModule::PlayerSettingsModule(
    rtype::client::PlayerSettings settings)
    : settings_(std::move(settings)), service_(settings_, storage_) {}

std::string PlayerSettingsModule::name() const {
  return std::string{rtype::client::SETTINGS_MODULE_NAME};
}

std::vector<std::string> PlayerSettingsModule::dependencies() const {
  return {};
}

void PlayerSettingsModule::install(rtype::engine::InstallContext& context) {
  context.provide(settings_);
  context.provide(service_);
}

void PlayerSettingsModule::start() {}

void PlayerSettingsModule::stop() {}

rtype::client::SettingsService& PlayerSettingsModule::service() {
  return service_;
}
