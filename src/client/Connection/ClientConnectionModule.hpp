#pragma once

#include <optional>
#include <string>
#include <vector>
#include "GameCatalog.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "MasterChoice.hpp"
#include "MasterLink.hpp"
#include "NetworkConditions.hpp"
#include "PlayerConnection.hpp"
#include "ServerCatalog.hpp"
#include "SettingValue.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief The module of the connection to a server: the PlayerConnection of the
 * client, whose parts it provides to the modules installed after it: the
 * ServerSession (the attempt and the session), the WaitingRoom, the
 * EntityMessageQueue, the ServerLeaderboard, IPlayerRoundTrips, the round
 * trip of each player, and IEntityTypeTable, the entity types of the server.
 * It also provides the network::TrafficCounters of its socket, and the lists
 * of the server map: IServerCatalog, the servers the master lists with their
 * ping, and IGameCatalog, the games of one of them, read from the master by a
 * MasterLink of its own and updated in NETWORK_STAGE; and IMasterAddress, the
 * master they are read from, which a MasterChoice decides: the one the player
 * chose, kept in PlayerSettings::master, or else the `master-url` setting.
 *
 * Building it reads the content of the assets folder, the client's own, and
 * keeps the packs of the servers it meets in PACK_CACHE_FOLDER_NAME of the
 * settings folder. install() draws the client's own content with the
 * EntityAppearances of the assets module, and declares the `master-url`
 * setting, defaultMasterUrl() until the configuration names another master.
 * It provides those parts, over a socket still closed, and adds to
 * NETWORK_STAGE the work that reads what the server sent and sends what is
 * due. start() opens the socket on every IPv4 interface, on a port the system
 * picks, and starts the network thread, then the thread of the MasterLink, on
 * the master of the MasterChoice once the configuration and the settings of
 * the player were read; when that one cannot start, it closes the socket again
 * before the exception leaves. It then subscribes the MasterChoice to the
 * SettingsService, so a master the player chooses is read at once. stop()
 * unsubscribes it, ends the MasterLink, then leaves the session with the
 * server, if any (see PlayerConnection::leave()): it returns once every copy of
 * the Disconnection went out, so the server frees the place of the player as
 * soon as one copy reaches it; then it closes the socket. Over a simulated
 * network, the copies still in flight then never arrive (see
 * NetworkThread::close()), and the server frees the place once the player was
 * silent for SESSION_SILENCE_LIMIT, counted in the ticks it runs, so longer
 * when the server stalls. A failure while leaving is logged, never thrown.
 */
class ClientConnectionModule final : public engine::IModule {
 public:
  /** @param conditions The network the datagrams of the client cross, read at
   * launch. */
  explicit ClientConnectionModule(const network::NetworkConditions& conditions);

  /** @returns CONNECTION_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns LOGGER_MODULE_NAME, since the connection logs from its first
   * frame on, ASSETS_MODULE_NAME, whose EntityAppearances it reads, and
   * SETTINGS_MODULE_NAME, whose PlayerSettings and SettingsService name the
   * master the player chose. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException when the assets module did not provide
   * the EntityAppearances. */
  void install(engine::InstallContext& context) override;
  /** @throws SocketOpenException when the system refuses to open the
   * socket. */
  void start() override;
  void stop() override;

 private:
  PlayerConnection connection_;
  SettingsService* settingsService_{nullptr};
  MasterLink masterLink_;
  ServerCatalog servers_;
  /** @brief Built by install(), once master-url is declared. */
  std::optional<MasterChoice> masterChoice_;
  GameCatalog games_;
  logging::Logger logger_;
};

}  // namespace rtype::client
