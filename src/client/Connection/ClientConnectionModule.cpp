#include "ClientConnectionModule.hpp"
#include <exception>
#include <filesystem>
#include <string>
#include <vector>
#include "AssetFolder.hpp"
#include "ClientConstants.hpp"
#include "ConnectionConstants.hpp"
#include "ContentLoadingConstants.hpp"
#include "ContentPlaces.hpp"
#include "ExecutablePath.hpp"
#include "IGameCatalog.hpp"
#include "IServerCatalog.hpp"
#include "InstallContext.hpp"
#include "MasterAccess.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ServerListConstants.hpp"
#include "SettingsFolder.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How the client reaches the master: its constant address and port,
 * with no key, since the reads of the list need none. */
network::MasterAccess clientMasterAccess() {
  return network::MasterAccess{
      .url = std::string{MASTER_URL_SCHEME} +
             std::string{network::LOOPBACK_IPV4_ADDRESS} +
             std::string{MASTER_PORT_SEPARATOR} +
             std::to_string(network::DEFAULT_MASTER_PORT),
      .key = std::string{}};
}

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
      masterLink_(clientMasterAccess()),
      servers_(masterLink_, connection_.pings()),
      games_(masterLink_),
      logger_(std::string{CONNECTION_LOGGER_NAME}) {}

std::string ClientConnectionModule::name() const {
  return std::string{CONNECTION_MODULE_NAME};
}

std::vector<std::string> ClientConnectionModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{ASSETS_MODULE_NAME}};
}

void ClientConnectionModule::install(engine::InstallContext& context) {
  connection_.provideParts(context);
  context.provide<IServerCatalog>(servers_);
  context.provide<IGameCatalog>(games_);
  context.addWork(NETWORK_STAGE, [this](engine::Duration elapsed) {
    connection_.update(elapsed);
    servers_.update(elapsed);
    games_.update();
  });
}

void ClientConnectionModule::start() {
  connection_.network().open();
  try {
    masterLink_.start();
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
