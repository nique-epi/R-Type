#include "ClientAssetsModule.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "AssetFolder.hpp"
#include "AssetLibrary.hpp"
#include "AssetLoad.hpp"
#include "ClientConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntitySheetConstants.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"

namespace rtype::client {

ClientAssetsModule::ClientAssetsModule(std::vector<AssetLoad> startupLoads)
    : startupLoads_(std::move(startupLoads)),
      logger_(std::string{CLIENT_LOGGER_NAME}) {}

std::string ClientAssetsModule::name() const {
  return std::string{ASSETS_MODULE_NAME};
}

std::vector<std::string> ClientAssetsModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ClientAssetsModule::install(engine::InstallContext& context) {
  library_ = std::make_unique<AssetLibrary>(engine::locateAssetFolder());
  appearances_ =
      std::make_unique<EntityAppearances>(EntityAppearances::fromFolder(
          library_->folder() / game::ENTITY_SHEET_FOLDER_IN_ASSETS));
  context.provide(*library_);
  context.provide(*appearances_);
}

void ClientAssetsModule::start() {
  logger_.info("assets folder: ", logging::genericText(library_->folder()));
  for (const AssetLoad& load : startupLoads_) {
    load(*library_);
  }
}

void ClientAssetsModule::stop() {}

}  // namespace rtype::client
