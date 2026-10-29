#include "ClientAssetsModule.hpp"
#include <memory>
#include <string>
#include <vector>
#include "AssetFolder.hpp"
#include "AssetIds.hpp"
#include "AssetLibrary.hpp"
#include "ClientConstants.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ScreenAssets.hpp"

namespace rtype::client {

ClientAssetsModule::ClientAssetsModule()
    : logger_(std::string{CLIENT_LOGGER_NAME}) {}

std::string ClientAssetsModule::name() const {
  return std::string{ASSETS_MODULE_NAME};
}

std::vector<std::string> ClientAssetsModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ClientAssetsModule::install(engine::InstallContext& context) {
  library_ = std::make_unique<AssetLibrary>(locateAssetFolder());
  context.provide(*library_);
}

void ClientAssetsModule::start() {
  logger_.info("assets folder: ", genericText(library_->folder()));
  loadInterfaceAssets(*library_);
  loadIntroAssets(*library_);
}

void ClientAssetsModule::stop() {}

}  // namespace rtype::client
