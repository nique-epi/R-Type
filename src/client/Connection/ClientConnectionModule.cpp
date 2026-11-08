#include "ClientConnectionModule.hpp"
#include <exception>
#include <filesystem>
#include <string>
#include <vector>
#include "AssetFolder.hpp"
#include "ClientConstants.hpp"
#include "ConfigurationRegistry.hpp"
#include "ConnectionConstants.hpp"
#include "ContentPackCacheConstants.hpp"
#include "ContentPlaces.hpp"
#include "ExecutablePath.hpp"
#include "IGameCatalog.hpp"
#include "IServerCatalog.hpp"
#include "InstallContext.hpp"
#include "MasterAccess.hpp"
#include "MasterLeaderboard.hpp"
#include "MasterUrlSetting.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConditions.hpp"
#include "SettingValue.hpp"
#include "SettingsFolder.hpp"
#include "TextSettingDefinition.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Where the client finds its own content, the assets folder, and
 * where it keeps the packs of servers: PACK_CACHE_FOLDER_NAME of its settings
 * folder, or of the folder of the executable when the system names none. */
ContentPlaces clientContentPlaces() {
  const std::filesystem::path settings =
      settingsFolder().value_or(engine::executableFolder());
  return ContentPlaces{.assetFolder = engine::locateAssetFolder(),
                       .cacheFolder = settings / PACK_CACHE_FOLDER_NAME};
}

}  // namespace

ClientConnectionModule::ClientConnectionModule(
    const network::NetworkConditions& conditions)
    : connection_(conditions, clientContentPlaces()),
      servers_(masterLink_, connection_.pings()),
      games_(masterLink_),
      globalLeaderboard_(masterLink_),
      logger_(std::string{CONNECTION_LOGGER_NAME}) {}

std::string ClientConnectionModule::name() const {
  return std::string{CONNECTION_MODULE_NAME};
}

std::vector<std::string> ClientConnectionModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{ASSETS_MODULE_NAME}};
}

void ClientConnectionModule::install(engine::InstallContext& context) {
  const engine::SettingValue& masterUrl =
      context.require<engine::ConfigurationRegistry>().declareText(
          engine::TextSettingDefinition{
              .name = std::string{MASTER_URL_SETTING_NAME},
              .defaultValue = defaultMasterUrl(),
              .help = "http(s)://<host>[:<port>] of the master the server map "
                      "reads",
              .validate = requireMasterUrl});
  masterUrl_ = &masterUrl;
  connection_.provideParts(context);
  context.provide<IServerCatalog>(servers_);
  context.provide<IGameCatalog>(games_);
  context.provide(globalLeaderboard_);
  context.addWork(NETWORK_STAGE, [this](engine::Duration elapsed) {
    connection_.update(elapsed);
    servers_.update(elapsed);
    games_.update();
    globalLeaderboard_.update();
  });
}

void ClientConnectionModule::start() {
  connection_.network().open();
  try {
    masterLink_.start(
        network::MasterAccess{.url = masterUrl_->text(), .key = std::string{}});
  } catch (const std::exception&) {
    connection_.network().close();
    throw;
  }
}

void ClientConnectionModule::stop() {
  masterLink_.stop();
  try {
    connection_.leave();
  } catch (const std::exception& error) {
    logger_.warn("the server was not told the client leaves: ", error.what());
  }
  connection_.network().close();
}

}  // namespace rtype::client
