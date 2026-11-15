#include "ClientCasinoModule.hpp"
#include <string>
#include <vector>
#include "ChipSettlement.hpp"
#include "ClientConstants.hpp"
#include "ICasinoChannel.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

std::string ClientCasinoModule::name() const {
  return std::string{CASINO_MODULE_NAME};
}

std::vector<std::string> ClientCasinoModule::dependencies() const {
  return {std::string{SCREENS_MODULE_NAME}, std::string{CONNECTION_MODULE_NAME},
          std::string{SETTINGS_MODULE_NAME}};
}

void ClientCasinoModule::install(engine::InstallContext& context) {
  casino_ = &context.require<ICasinoChannel>();
  settings_ = &context.require<SettingsService>();
  context.addWork(UPDATE_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    settleChipMoves(*casino_, *settings_);
                  });
}

void ClientCasinoModule::start() {}

void ClientCasinoModule::stop() {}

}  // namespace rtype::client
