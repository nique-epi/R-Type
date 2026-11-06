#pragma once

#include <optional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IServerName.hpp"
#include "IServerOccupancy.hpp"
#include "InstallContext.hpp"
#include "ServerInformationDesk.hpp"
#include "SettingValue.hpp"
#include "SystemClock.hpp"
#include "TickDatagrams.hpp"

namespace rtype::server {

/**
 * @brief The module information, required: it owns the server-name setting,
 * provides it as the IServerName the master module registers, and answers the
 * information requests of each tick with a ServerInformationDesk.
 *
 * It is a module of the supervisor: the occupancy it answers is the one the
 * supervised games module counts (IServerOccupancy). Its work is in the
 * simulation stage, after that module's: the games are supervised first, so
 * an answer counts the players and the free seats their newest statuses
 * gave. The name is final once the configuration was read, so
 * start() builds the desk.
 */
class ServerInformationModule final : public engine::IModule,
                                      public IServerName {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Declares server-name, reads the services and provides the
   * IServerName; opens nothing.
   * @throws MissingServiceException when no module provided the occupancy,
   * the sender or the datagrams of the tick.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Builds the desk with the name the configuration gave. */
  void start() override;

  /** @brief Drops the desk. */
  void stop() override;

  /** @returns The server-name setting, DEFAULT_SERVER_NAME until the
   * configuration sets another. */
  [[nodiscard]] std::string serverName() const override;

 private:
  void declareSettings(engine::ConfigurationRegistry& configuration);
  void answerInformationRequests();

  engine::SystemClock clock_;
  const engine::SettingValue* nameSetting_{nullptr};
  const IServerOccupancy* occupancy_{nullptr};
  IDatagramSender* sender_{nullptr};
  const TickDatagrams* tickDatagrams_{nullptr};
  std::optional<ServerInformationDesk> desk_;
};

}  // namespace rtype::server
